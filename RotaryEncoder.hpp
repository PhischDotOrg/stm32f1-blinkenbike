/*-
 * $Copyright$
-*/

#ifndef _ROTARY_ENCODER_HPP_879BE2DA_C737_495B_8569_1829995D9DC2
#define _ROTARY_ENCODER_HPP_879BE2DA_C737_495B_8569_1829995D9DC2

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
    intptr_t AddressT,
    typename RccT,
    typename NvicT,
    typename PinT
>
static void
initRotaryEncoderTimer(const RccT &p_rcc, const NvicT &p_nvic, const PinT &p_rotaryCW, const PinT &p_rotaryCCW, uint16_t p_maxCounter) {
    TIM_TypeDef * const p_tim = reinterpret_cast<TIM_TypeDef *>(AddressT);
    const stm32::EngineT<AddressT> * const engine = nullptr;

    p_rcc.enableEngine(*engine);
    p_nvic.enableIrq(*engine);

    p_rotaryCW.selectAlternateFn(static_cast<const stm32::EngineT<AddressT> &>(*engine));
    p_rotaryCCW.selectAlternateFn(static_cast<const stm32::EngineT<AddressT> &>(*engine));

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

template<
    typename BlinkenBikeT
>
static void
handleRotaryEncoderTimerIrq(TIM_TypeDef * const p_tim, BlinkenBikeT &p_blinkenBike) {
    /*
     * TIF Irq is triggered when there is an edge (rising and falling) on the TIMx_CH1 / TI1 Pin.
     * DIR Bit is set if knob is turned clockwise (downcounter).
     * DIR Bit is cleared, if knob is turned counter-clockwise (upcounter).
     * Therefore, CNT counts down if knob is turned clockwise and up if knob is turned counter-clockwise.
     */
    if (p_tim->SR & TIM_SR_TIF) {
        p_blinkenBike.knobTurned(p_tim->CNT, (p_tim->CR1 & TIM_CR1_DIR) >> TIM_CR1_DIR_Pos);

        p_tim->SR &= ~TIM_SR_TIF;
    }

    p_tim->SR = 0;
}

#endif /* _ROTARY_ENCODER_HPP_879BE2DA_C737_495B_8569_1829995D9DC2 */
