/*-
 * $Copyright$
-*/
#include <common/Infrastructure.hpp>
#include <phisch/log.h>

/* for vTaskStartScheduler */
#include <FreeRTOS.h>
#include <FreeRTOS/include/task.h>

#include <stm32/Cpu.hpp>

#include <stm32/Pll.hpp>
#include <stm32/Pwr.hpp>
#include <stm32/Flash.hpp>
#include <stm32/Gpio.hpp>
#include <stm32/Rcc.hpp>
#include <stm32/Scb.hpp>
#include <stm32/Nvic.hpp>

#include <gpio/GpioAccess.hpp>
#include <gpio/GpioEngine.hpp>
#include <gpio/GpioPin.hpp>

#include <stm32/Uart.hpp>
#include <uart/UartAccess.hpp>
#include <uart/UartDevice.hpp>

#include <tasks/Heartbeat.hpp>

#include <stm32f1/dma/Engine.hpp>
#include <stm32f1/dma/Channel.hpp>
#include <stm32f1/dma/Stream.hpp>

#include <stm32/Spi.hpp>
#include <spi/SpiAccess.hpp>
#include <spi/SpiDevice.hpp>
#include <devices/Ws2812bStrip.hpp>

#include "ShutdownHandler.hpp"

/*******************************************************************************
 * System Devices
 ******************************************************************************/
static const constexpr stm32::PllCfg pllCfg = {
    .m_pllSource        = stm32::PllCfg::PllSource_t::e_PllSourceHSE,
    .m_hseSpeedInHz     = 4 * 1000 * 1000,
    .m_hsePrescaler     = stm32::PllCfg::HSEPrescaler_t::e_HSEPrescaler_None,
    .m_pllM             = stm32::PllCfg::PllMul_t::e_PllM_13,
    .m_sysclkSource     = stm32::PllCfg::SysclkSource_t::e_SysclkPLL,
    .m_ahbPrescaler     = stm32::PllCfg::AHBPrescaler_t::e_AHBPrescaler_None,
    .m_apb1Prescaler    = stm32::PllCfg::APBPrescaler_t::e_APBPrescaler_Div16,
    .m_apb2Prescaler    = stm32::PllCfg::APBPrescaler_t::e_APBPrescaler_None
};

static stm32::Scb                       scb(SCB);
static stm32::Nvic                      nvic(NVIC, scb);

static stm32::Flash                     flash(FLASH);
static stm32::Rcc                       rcc(RCC, pllCfg, flash);
static stm32::Pwr                       pwr(rcc, scb);

/*******************************************************************************
 * GPIO Engine Handlers
 ******************************************************************************/
static stm32::Gpio::A                   gpio_A(rcc);
static gpio::GpioEngine                 gpio_engine_A(&gpio_A);

static stm32::Gpio::B                   gpio_B(rcc);
static gpio::GpioEngine                 gpio_engine_B(&gpio_B);

static stm32::Gpio::C                   gpio_C(rcc);
static gpio::GpioEngine                 gpio_engine_C(&gpio_C);

/*******************************************************************************
 * LEDs
 ******************************************************************************/
static gpio::AlternateFnPin             g_mco1(gpio_engine_A, 8);
static gpio::GpioPin                    g_led_green(gpio_engine_C, 13);

/*******************************************************************************
 * UART
 ******************************************************************************/
static gpio::AlternateFnPin             uart_tx(gpio_engine_A, 9);
static gpio::AlternateFnPin             uart_rx(gpio_engine_A, 10);
static stm32::Uart::Usart1<gpio::AlternateFnPin>    uart_access(rcc, uart_rx, uart_tx);
uart::UartDevice                        g_uart(&uart_access);

/*******************************************************************************
 * DMA Engines
 ******************************************************************************/
static stm32::Dma::Engine               dma_1(rcc);

/*******************************************************************************
 * DMA Streams and Channels
 ******************************************************************************/
static stm32::Dma::Channel<2>           ws2812b_spiRxDmaChannel(nvic, dma_1);
static stm32::Dma::Channel<3>           ws2812b_spiTxDmaChannel(nvic, dma_1);

/*******************************************************************************
 * WS2812b Strip
 ******************************************************************************/
static gpio::AlternateFnPin spi_sclk(gpio_engine_A, 5);
static gpio::AlternateFnPin spi_nsel(gpio_engine_A, 4);
static gpio::AlternateFnPin spi_mosi(gpio_engine_A, 7);
static gpio::AlternateFnPin spi_miso(gpio_engine_A, 6);

#if defined(SPI_VIA_DMA)
static stm32::Spi::DmaSpi1<
    gpio::AlternateFnPin,
    decltype(ws2812b_spiTxDmaChannel),
    decltype(ws2812b_spiRxDmaChannel)
>
#else
static stm32::Spi::Spi1<
    gpio::AlternateFnPin
>
#endif
ws2812b_spibus( /* p_rcc = */ rcc,
                                    #if defined(SPI_VIA_DMA)
                                        /* p_txDmaChannel = */ ws2812b_spiTxDmaChannel,
                                        /* p_rxDmaChannel = */ ws2812b_spiRxDmaChannel,
                                    #endif
                                        /* p_sclk = */ spi_sclk,
                                        /* p_nsel = */ spi_nsel,
                                        /* p_mosi = */ spi_mosi,
                                        /* p_miso = */ spi_miso,
                                        /* p_prescaler = */ decltype(ws2812b_spibus)::BaudRatePrescaler_e::e_SpiPrescaler16);

static spi::DeviceT<decltype(ws2812b_spibus)>               ws2812b_spidev(&ws2812b_spibus);
static devices::Ws2812bStripT<
    1,
    decltype(ws2812b_spidev),
    devices::Ws2812bDataNonInverted
>                                                           ws2812bStrip(ws2812b_spidev);

/*******************************************************************************
 * Rotary Encoder
 ******************************************************************************/
static gpio::AlternateFnInPinT<gpio::PinPolicy::Termination_e::e_None>      g_rotaryCW(gpio_engine_B, 6);   // TIM4_CH1
static gpio::AlternateFnInPinT<gpio::PinPolicy::Termination_e::e_None>      g_rotaryCCW(gpio_engine_B, 7);  // TIM4_CH2
static gpio::AlternateFnInPinT<gpio::PinPolicy::Termination_e::e_PullUp>    g_rotaryPush(gpio_engine_A, 0); // TIM2_CH1

/*******************************************************************************
 * Tasks
 ******************************************************************************/
static tasks::HeartbeatT<decltype(g_led_green)> heartbeat_gn("hrtbt_g", g_led_green, 3, 500);

#if 1
/* FIXME Only for testing the WS2812B Driver via SPI and DMA. */
static bool led_status = false;

int
toggleLed(void *p_data) {
    assert(p_data != nullptr);

    bool *status = static_cast<bool *>(p_data);

    if (*status) {
        ws2812bStrip.setPixel(0, FastLED::CRGB(0x0F0000));
    } else {
        ws2812bStrip.setPixel(0, FastLED::CRGB(0x000F00));
    }
    ws2812bStrip.show();

    *status = !(*status);

    return (0);
}

static tasks::PeriodicCallback  task_500ms("t_500ms", 2, 500, &toggleLed, &led_status);
#endif

static tasks::ShutdownHandler  shutdownHandler("shutdown", pwr, ws2812bStrip, 2);

/*******************************************************************************
 * Rotary Encoder Timer and IRQ Handler
 ******************************************************************************/

/*
 * Configure the Timer in Encoder Mode.
 *
 * Encoder Pin that is changing first ("CLK" or "CW") should be connected to
 * Timer Channel 2.
 *
 * Encoder Pin that is changing later ("DT" or "CCW") should be connected to
 * Timer Channel 1.
 *
 * Timer is configured to count when an edge on TI2FP1, i.e. Timer Channel 2 is
 * detected. The direction of the counter will depend on the signal level on the
 * Timer Channel 1 input pin.
 *
 * In the Timer Hardware, Timer Channel 1 generates the TI1F_ED signal which
 * indicates any edge (rising or falling) on the input pin. This signal can be
 * used to trigger the counter.
 *
 * The Timer is configured raise an interrupt when triggered.
 *
 * This means when the knob is turned clockwise:
 * - The "CW" Pin changes, causing an edge on TI2FP1.
 * - The edge on TI2FP1 causes the counter to count downwards.
 *     - If "CW" changes from 'low' to 'high', i.e. there is a rising edge,
 *       this means that "CCW" must still be 'low'.
 *     - If "CW" changes from 'high' to 'low', i.e. there is a falling edge,
 *       this means that "CCW" must still be 'high'.
 *     - See Table #85 in the STM32F103 CPU Reference Manual.
 * - When the knob is turned further, the "CCW" Pin changes, causing an
 *   edge on TI1FP2.
 *  - The edge on TI1FP2 triggers the counter, causing an interrupt (TIF).
 *  - In the Timer IRQ Handler, Software can read the current position of the
 *    knob in the Timer's CNT register.
 */
template<
    intptr_t AddressT
>
static void
initRotaryEncoderTimer(uint16_t p_maxCounter) {
    TIM_TypeDef * const p_tim = reinterpret_cast<TIM_TypeDef *>(AddressT);
    const stm32::EngineT<AddressT> * const engine = nullptr;

    rcc.enableEngine(*engine);
    nvic.enableIrq(*engine);

    g_rotaryCW.selectAlternateFn(static_cast<const stm32::EngineT<AddressT> &>(*engine));
    g_rotaryCCW.selectAlternateFn(static_cast<const stm32::EngineT<AddressT> &>(*engine));

    /*
     * From TIM_Base_SetConfig()
     */
    /* Auto-reload preload enable: TIMx_ARR register is buffered */
    p_tim->CR1 &= ~(TIM_CR1_ARPE_Msk);
    p_tim->CR1 |= (1 << TIM_CR1_ARPE_Pos);

    /* Clock Divison */
    p_tim->CR1 |= (0b10 << TIM_CR1_CKD_Pos);

    /* Auto-Reload Register */
    p_tim->ARR = p_maxCounter - 1;

    /* Prescaler */
    p_tim->PSC = 0;

    /* Update / Re-initialize Counter */
    p_tim->EGR |= TIM_EGR_UG;

    /*
     * From HAL_TIM_Encoder_Init()
     */
    /* Step 1: Set Encoder Mode -- SMS[2:0] = 0b011 */
    p_tim->SMCR  &= ~(TIM_SMCR_SMS_Msk);

    /*
     * b001 -- Encoder Mode #1 -- Counter ticks on TI2FP1 Edge
     * b010 -- Encoder Mode #2 -- Counter ticks on TI1FP2 Edge
     * b011 -- Encoder Mode #3 -- Counter ticks on both TI1FP2 and TI1FP1 Edge.
     *
     * Counter should count on TI2FP1.
     */
    p_tim->SMCR  |= (0b001 << TIM_SMCR_SMS_Pos);

    /* Step 2: Capture/Compare Selection: CC{1,2} configured as Input, IC{1,2} mapped to TI{1,2} */
    p_tim->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk);
    p_tim->CCMR1 |= (0b01 << TIM_CCMR1_CC1S_Pos);

    p_tim->CCMR1 &= ~(TIM_CCMR1_CC2S_Msk);
    p_tim->CCMR1 |= (0b01 << TIM_CCMR1_CC2S_Pos);

    /* Step 3: Configure Prescaler as Disabled, i.e. no Prescaler */
    p_tim->CCMR1 &= ~(TIM_CCMR1_IC1PSC_Msk);
    p_tim->CCMR1 |= (0 << TIM_CCMR1_IC1PSC_Pos);

    p_tim->CCMR1 &= ~(TIM_CCMR1_IC1PSC_Msk);
    p_tim->CCMR1 |= (0 << TIM_CCMR1_IC1PSC_Pos);

    /* Step 4: Configure Input Capture Filter */
    unsigned filter = 0b1111;

    p_tim->CCMR1 &= ~(TIM_CCMR1_IC1F_Msk);
    p_tim->CCMR1 |= (filter << TIM_CCMR1_IC1F_Pos);

    p_tim->CCMR1 &= ~(TIM_CCMR1_IC2F_Msk);
    p_tim->CCMR1 |= (filter << TIM_CCMR1_IC2F_Pos);

    /* Step 5: Configure Input Channel Polarity */
    p_tim->CCER  &= ~(TIM_CCER_CC1P_Msk);
    p_tim->CCER  |= (0 << TIM_CCER_CC1P_Pos);

    p_tim->CCER  &= ~(TIM_CCER_CC2P_Msk);
    p_tim->CCER  |= (0 << TIM_CCER_CC2P_Pos);

    /* Step 6: Enable Timer Channels */
    p_tim->CCER  |= (TIM_CCER_CC1E | TIM_CCER_CC2E);

    /*
     * Trigger Selection -- TI1F_ED Signal
     *
     * 0b100 -- Trigger on TI1F_ED
     * 0b101 -- Trigger on TI1FP1
     * 0b110 -- Trigger on TI1FP2
     */
    p_tim->SMCR |= (0b100) << TIM_SMCR_TS_Pos;
    p_tim->DIER |= (TIM_DIER_TIE /* | TIM_DIER_CC1IE | TIM_DIER_CC2IE */);

    /* Go! */
    p_tim->CR1 |= TIM_CR1_CEN;
}


static void
handleRotaryEncoderTimerIrq(TIM_TypeDef * const p_tim) {
    /*
     * TIF Irq is triggered when there is an edge (rising and falling) on the TIMx_CH1 / TI1 Pin.
     * DIR Bit is set if knob is turned clockwise (downcounter).
     * DIR Bit is cleared, if knob is turned counter-clockwise (upcounter).
     * Therefore, CNT counts down if knob is turned clockwise and up if knob is turned counter-clockwise.
     */
    if (p_tim->SR & TIM_SR_TIF) {
        PHISCH_LOG("%s() TIM_SR_TIF CNT=%d DIR=%d\r\n", __func__, p_tim->CNT, (p_tim->CR1 & TIM_CR1_DIR) >> TIM_CR1_DIR_Pos);

        p_tim->SR &= ~TIM_SR_TIF;
    }

    p_tim->SR = 0;
}

/*******************************************************************************
 * Push Button Timer and IRQ Handler
 ******************************************************************************/
template<
    intptr_t AddressT
>
static void
initPushButtonTimer(void) {
    TIM_TypeDef * const tim = reinterpret_cast<TIM_TypeDef *>(AddressT);
    const stm32::EngineT<AddressT> * const engine = nullptr;

    rcc.enableEngine(*engine);
    nvic.enableIrq(*engine);

    g_rotaryPush.selectAlternateFn(static_cast<const stm32::EngineT<AddressT> &>(*engine));

    /* Auto-reload preload enable: TIMx_ARR register is buffered */
    tim->CR1 &= ~(TIM_CR1_ARPE_Msk);
    tim->CR1 |= (1 << TIM_CR1_ARPE_Pos);

    /* Clock Divison */
    tim->CR1 |= (0b10 << TIM_CR1_CKD_Pos);

    /* Auto-Reload Register */
    tim->ARR = 0;

    /* Prescaler */
    tim->PSC = -1;

    /* Update / Re-initialize Counter */
    tim->EGR |= TIM_EGR_UG;

    /* Set up TIM CC3 input */
    unsigned filter = 0b1111;

    tim->CCMR1 |= (0b01 >> TIM_CCMR1_CC1S_Pos);

    tim->CCMR1 &= ~(TIM_CCMR1_IC1F_Msk);
    tim->CCMR1 |= (filter << TIM_CCMR1_IC1F_Pos);

    /* Configure Input Channel Polarity -- Rising Edge */
    tim->CCER  &= ~TIM_CCER_CC1P;

    /* Configure Input Channel Capture */
    tim->CCER  |= TIM_CCER_CC1E;

    tim->DIER |= TIM_DIER_CC1IE;

    /* Go! */
    tim->CR1 |= TIM_CR1_CEN;
}

static void
handlePushButtonTimerIrq(TIM_TypeDef * const p_tim) {
    if (p_tim->SR & TIM_SR_CC1IF) {
        if (p_tim->CCER & TIM_CCER_CC1P) {
            PHISCH_LOG("%s(): Release\r\n", __func__);
            shutdownHandler.notifyIrq(decltype(shutdownHandler)::Shutdown_e::e_Stop);
        } else {
            PHISCH_LOG("%s(): Push\r\n", __func__);
            shutdownHandler.notifyIrq(decltype(shutdownHandler)::Shutdown_e::e_Start);
        }

        p_tim->CCER ^= TIM_CCER_CC1P;
    }

    p_tim->SR = 0;
}

static int
task100ms(void * /* p_data */) {
    return (0);
}

static tasks::PeriodicCallback  task_100ms("t_100ms", 2, 100, &task100ms, nullptr);

/*******************************************************************************
 *
 ******************************************************************************/
const uint32_t SystemCoreClock = pllCfg.getSysclkSpeedInHz();

static_assert(pllCfg.isValid() == true,                             "PLL Configuration is not valid!");
static_assert(SystemCoreClock               ==  52 * 1000 * 1000,   "Expected System Clock to be at 52 MHz!");
static_assert(pllCfg.getAhbSpeedInHz()      ==  52 * 1000 * 1000,   "Expected AHB to be running at  52 MHz!");
static_assert(pllCfg.getApb1SpeedInHz()     ==       3250 * 1000,   "Expected APB1 to be running at 3,25 MHz!");
static_assert(pllCfg.getApb2SpeedInHz()     ==  52 * 1000 * 1000,   "Expected APB2 to be running at 52 MHz!");

/*******************************************************************************
 *
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

int
main(void) {
    rcc.setMCO(g_mco1, decltype(rcc)::MCOOutput_e::e_HSE, decltype(rcc)::MCOPrescaler_t::e_MCOPre_None);

    uart_access.setBaudRate(decltype(uart_access)::BaudRate_e::e_230400);

    const unsigned sysclk = pllCfg.getSysclkSpeedInHz() / 1000;
    const unsigned ahb    = pllCfg.getAhbSpeedInHz() / 1000;
    const unsigned apb1   = pllCfg.getApb1SpeedInHz() / 1000;
    const unsigned apb2   = pllCfg.getApb2SpeedInHz() / 1000;

    PrintStartupMessage(sysclk, ahb, apb1, apb2);

    /*
     * Enable the WKUP Pin here to ensure consistent behavior between cold start-up, i.e. after
     * initially applying the Supply Voltage vs. waking up after Stand-by.
     */
    pwr.enableWkup();

    PHISCH_LOG("PWR::CSR = 0x%x\r\n", PWR->CSR);

    if (SysTick_Config(SystemCoreClock / 1000)) {
        PHISCH_LOG("FATAL: Capture Error!\r\n");
        goto bad;
    }

    initRotaryEncoderTimer<TIM4_BASE>(5);
    initPushButtonTimer<TIM2_BASE>();

    PHISCH_LOG("Starting FreeRTOS Scheduler...\r\n");
    vTaskStartScheduler();

bad:
    PHISCH_LOG("FATAL ERROR!\r\n");
    while (1) ;

    return (0);
}

/*******************************************************************************
 * Interrupt Handlers
 ******************************************************************************/
void
SPI1_IRQHandler(void) {
    ws2812b_spibus.handleIrq();
}

void
DMA1_Channel2_IRQHandler(void) {
    ws2812b_spiRxDmaChannel.handleIrq();
}

void
DMA1_Channel3_IRQHandler(void) {
    ws2812b_spiTxDmaChannel.handleIrq();
}

void
TIM4_IRQHandler(void) {
    handleRotaryEncoderTimerIrq(reinterpret_cast<TIM_TypeDef *>(TIM4_BASE));
}

void
TIM2_IRQHandler(void) {
    handlePushButtonTimerIrq(reinterpret_cast<TIM_TypeDef *>(TIM2_BASE));
}

#if defined(__cplusplus)
} /* extern "C" */
#endif /* defined(__cplusplus) */
