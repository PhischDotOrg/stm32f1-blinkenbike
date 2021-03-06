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
#include "RotaryEncoder.hpp"
#include "PushButton.hpp"
#include "BlinkenBike.hpp"

/*******************************************************************************
 * System Devices
 ******************************************************************************/
static const constexpr stm32::PllCfg pllCfg = {
    .m_pllSource        = stm32::PllCfg::PllSource_t::e_PllSourceHSI,
    .m_hseSpeedInHz     = 0,
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
 * WS2812b Strip
 ******************************************************************************/
static gpio::AlternateFnPin spi_sclk(gpio_engine_A, 5);
static gpio::AlternateFnPin spi_nsel(gpio_engine_A, 4);
static gpio::AlternateFnPin spi_mosi(gpio_engine_A, 7);
static gpio::AlternateFnPin spi_miso(gpio_engine_A, 6);

static stm32::Spi::Spi1<
    gpio::AlternateFnPin
>
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
    1 + 18,
    decltype(ws2812b_spidev),
    devices::Ws2812bDataInverted
>                                                           ws2812bStrip(ws2812b_spidev);

/*******************************************************************************
 * Main Blinkenlight State Machine Logic
 ******************************************************************************/
static BlinkenBikeT blinkenBike(ws2812bStrip);

/*******************************************************************************
 * Rotary Encoder
 ******************************************************************************/
static gpio::AlternateFnInPinT<gpio::PinPolicy::Termination_e::e_None>      g_rotaryCW(gpio_engine_B, 6);   // TIM4_CH1
static gpio::AlternateFnInPinT<gpio::PinPolicy::Termination_e::e_None>      g_rotaryCCW(gpio_engine_B, 7);  // TIM4_CH2
static gpio::AlternateFnInPinT<gpio::PinPolicy::Termination_e::e_PullUp>    g_rotaryPush(gpio_engine_A, 0); // TIM2_CH1

/*******************************************************************************
 * Tasks
 ******************************************************************************/
static tasks::HeartbeatT                        heartbeat_gn("hrtbt_g", g_led_green, 1, 500);
static tasks::ShutdownHandlerT                  shutdownHandler("shutdown", pwr, ws2812bStrip, 3);

int
refreshLedStrip(void * /* p_data */) {
    blinkenBike.blinkTick();

    taskDISABLE_INTERRUPTS();
    ws2812bStrip.show();
    taskENABLE_INTERRUPTS();

    return (0);
}

static tasks::PeriodicCallback  task_25ms("t_25ms", 2, 25, &refreshLedStrip);

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

    if (SysTick_Config(SystemCoreClock / 1000)) {
        PHISCH_LOG("FATAL: Capture Error!\r\n");
        goto bad;
    }

    initRotaryEncoderTimer<TIM4_BASE>(rcc, nvic, g_rotaryCW, g_rotaryCCW, 5);
    initPushButtonTimer<TIM2_BASE>(rcc, nvic, g_rotaryPush);

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
TIM4_IRQHandler(void) {
    handleRotaryEncoderTimerIrq(reinterpret_cast<TIM_TypeDef *>(TIM4_BASE), blinkenBike);
}

void
TIM2_IRQHandler(void) {
    handlePushButtonTimerIrq(reinterpret_cast<TIM_TypeDef *>(TIM2_BASE), shutdownHandler, blinkenBike);
}

#if defined(__cplusplus)
} /* extern "C" */
#endif /* defined(__cplusplus) */
