/*-
 * $Copyright$
-*/

#ifndef _BLINKENBIKE_HPP_BEF35430_91D6_4F5E_9587_EF88B244E219
#define _BLINKENBIKE_HPP_BEF35430_91D6_4F5E_9587_EF88B244E219

#include <FreeRTOS.h>

#include <FastLED/pixeltypes.h>

/*****************************************************************************/
template<
    typename LedStripT
>
class
BlinkenBikeT {
    enum class InputMode_t : uint8_t {
        e_Color,                // Turning Knob will change color of LEDs.
        e_Brightness,           // Turning Knob will change Brightness of LEDs.
        e_Speed,                // Turning Knob will change speed of Animation.
    };

    enum class OutputMode_t : uint8_t {
        e_Solid,                // Dauerlicht
        e_Flash,                // Blinken
        e_UpDownSynchronous,    // Hoch/Runter gleiche Richtung
        e_UpDownOpposite,       // Hoch/Runter gegenlaeufig
        e_Upwards,              // Nur Hoch
        e_Downwards,            // Nur Runter
        e_EvenOdd,              // Blinken m. jeder zweiten LED
    };

    typedef struct State_s {
        OutputMode_t            m_outputMode;
        InputMode_t             m_inputMode;
        HSVHue                  m_stateColor;
    } State_t;

    static constexpr unsigned   m_uiLed = 0;
    static constexpr State_t    m_states[8] /* __attribute__((aligned(4), section(".fixeddata"))) */ = {
        { .m_outputMode = OutputMode_t::e_Solid,                .m_inputMode = InputMode_t::e_Color,        .m_stateColor = HSVHue::HUE_RED },
        { .m_outputMode = OutputMode_t::e_Solid,                .m_inputMode = InputMode_t::e_Brightness,   .m_stateColor = HSVHue::HUE_ORANGE },
        { .m_outputMode = OutputMode_t::e_Flash,                .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = HSVHue::HUE_YELLOW },
        { .m_outputMode = OutputMode_t::e_UpDownSynchronous,    .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = HSVHue::HUE_GREEN },
        { .m_outputMode = OutputMode_t::e_UpDownOpposite,       .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = HSVHue::HUE_AQUA },
        { .m_outputMode = OutputMode_t::e_Upwards,              .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = HSVHue::HUE_BLUE },
        { .m_outputMode = OutputMode_t::e_Downwards,            .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = HSVHue::HUE_PURPLE },
        { .m_outputMode = OutputMode_t::e_EvenOdd,              .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = HSVHue::HUE_PINK },
    };
    static constexpr unsigned   m_nStates = sizeof(m_states) / sizeof(m_states[0]);

    LedStripT &                 m_ledStrip;
    unsigned                    m_currentState;

public:
    BlinkenBikeT(LedStripT &p_ledStrip) : m_ledStrip(p_ledStrip), m_currentState(0) {
        // static_assert(LedStripT::SIZE > 1, "LED Strip should be more than one LED long!");
        static_assert((LedStripT::SIZE % 2) == 1, "LED Strip should be 1 + an even number of LEDs long!");
        static_assert(m_nStates == 8);
    }

    void
    changeState(void) {
        m_currentState = (m_currentState + 1) % m_nStates;

        HSVHue  uiColor = m_states[m_currentState].m_stateColor;
        CRGB    uiPixel(uiColor);

        uiPixel.setHue(uiColor);

        m_ledStrip.setPixel(m_uiLed, uiPixel);
    }
};
/*****************************************************************************/

#endif /* _BLINKENBIKE_HPP_BEF35430_91D6_4F5E_9587_EF88B244E219 */
