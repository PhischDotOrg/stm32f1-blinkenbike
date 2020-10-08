/*-
 * $Copyright$
-*/

#ifndef _PUSHBUTTON_HPP_69A07982_5EEC_4547_B595_553EB49B1563
#define _PUSHBUTTON_HPP_69A07982_5EEC_4547_B595_553EB49B1563

template<
    intptr_t AddressT,
    typename RccT,
    typename NvicT,
    typename PinT
>
static void
initPushButtonTimer(const RccT &p_rcc, const NvicT &p_nvic, const PinT &p_rotaryPush) {
    TIM_TypeDef * const tim = reinterpret_cast<TIM_TypeDef *>(AddressT);
    const stm32::EngineT<AddressT> * const engine = nullptr;

    p_rcc.enableEngine(*engine);
    p_nvic.enableIrq(*engine);

    p_rotaryPush.selectAlternateFn(static_cast<const stm32::EngineT<AddressT> &>(*engine));

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

template<
    typename ShutdownHandlerT,
    typename BlinkenBikeT
>
static void
handlePushButtonTimerIrq(TIM_TypeDef * const p_tim, const ShutdownHandlerT &p_shutdownHandler, BlinkenBikeT &p_blinkenBike) {
    if (p_tim->SR & TIM_SR_CC1IF) {
        if (p_tim->CCER & TIM_CCER_CC1P) {
            p_shutdownHandler.notifyIrq(ShutdownHandlerT::Shutdown_e::e_Stop);
            p_blinkenBike.changeState();
        } else {
            p_shutdownHandler.notifyIrq(ShutdownHandlerT::Shutdown_e::e_Start);
        }

        p_tim->CCER ^= TIM_CCER_CC1P;
    }

    p_tim->SR = 0;
}

#endif /* _PUSHBUTTON_HPP_69A07982_5EEC_4547_B595_553EB49B1563 */
