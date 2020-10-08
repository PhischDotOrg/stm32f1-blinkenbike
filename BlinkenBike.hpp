/*-
 * $Copyright$
-*/

#ifndef _BLINKENBIKE_HPP_BEF35430_91D6_4F5E_9587_EF88B244E219
#define _BLINKENBIKE_HPP_BEF35430_91D6_4F5E_9587_EF88B244E219

#include <phisch/log.h>

#include <FreeRTOS.h>

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
        Pixel::HSV::Hue         m_stateColor;
    } State_t;

    static constexpr unsigned   m_uiLed = 0;
    static constexpr State_t    m_states[] __attribute__((aligned(4), section(".fixeddata"))) = {
        { .m_outputMode = OutputMode_t::e_Solid,                .m_inputMode = InputMode_t::e_Color,        .m_stateColor = Pixel::HSV::Hue::e_Red },
        { .m_outputMode = OutputMode_t::e_Solid,                .m_inputMode = InputMode_t::e_Brightness,   .m_stateColor = Pixel::HSV::Hue::e_Orange },
        { .m_outputMode = OutputMode_t::e_Flash,                .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = Pixel::HSV::Hue::e_Yellow },
        { .m_outputMode = OutputMode_t::e_UpDownSynchronous,    .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = Pixel::HSV::Hue::e_Green },
        { .m_outputMode = OutputMode_t::e_UpDownOpposite,       .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = Pixel::HSV::Hue::e_Turquoise },
        { .m_outputMode = OutputMode_t::e_Upwards,              .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = Pixel::HSV::Hue::e_Blue },
        { .m_outputMode = OutputMode_t::e_Downwards,            .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = Pixel::HSV::Hue::e_Purple },
        { .m_outputMode = OutputMode_t::e_EvenOdd,              .m_inputMode = InputMode_t::e_Speed,        .m_stateColor = Pixel::HSV::Hue::e_Magenta },
    };
    static constexpr unsigned   m_nStates = sizeof(m_states) / sizeof(m_states[0]);

    static constexpr unsigned   m_maxBrightness = 64;

    LedStripT &                 m_ledStrip;
    unsigned                    m_currentState;

    Pixel::HSV                  m_uiHsv;
    bool                        m_uiChanged;

    Pixel::HSV                  m_hsvColor;
    Pixel::RGB                  m_rgbColor;
    bool                        m_colorChanged;

    int8_t                      m_counter;
    static constexpr uint8_t    m_maxCounter = 40;

    int8_t                      m_speed;
    static constexpr uint8_t    m_minSpeed = 1;
    static constexpr uint8_t    m_maxSpeed = (m_maxCounter / 3) + 1;

    /* Flash State */
    bool                        m_flash;

    template<typename ValueT, typename DistanceT>
    struct Knob_s {
        ValueT      m_previous;
        DistanceT   m_distance;

        Knob_s() : m_previous(0), m_distance(0) {

        }

        void update(ValueT p_new, bool p_directionCw) {
            m_distance = (p_directionCw ? 1 : -1);

            m_previous = p_new;
        }
 
        DistanceT distance(void) const {
            return m_distance;
        }

        static constexpr unsigned m_maxValue = (1 << ((sizeof(ValueT) * 8)) - 1) - 1;
        static_assert(sizeof(DistanceT) >= sizeof(ValueT));
    };

    struct Knob_s<uint8_t, int8_t>  m_knob;

    InputMode_t
    getCurrentInputMode(void) const {
        return m_states[m_currentState].m_inputMode;
    }

    OutputMode_t
    getCurrentOutputMode(void) const {
        return m_states[m_currentState].m_outputMode;
    }

    void
    changeBrightness(int8_t p_distance) {
        m_hsvColor.val = (m_hsvColor.val + p_distance) % m_maxBrightness;

        m_rgbColor = m_hsvColor;
        m_colorChanged = true;
    }

    void
    changeColor(int8_t p_distance) {
        m_hsvColor.hue += 160 * p_distance;

        m_rgbColor = m_hsvColor;
        m_colorChanged = true;
    }

    void
    changeSpeed(int8_t p_distance) {
        m_speed += 4 * p_distance;

        if (m_speed < m_minSpeed) {
            m_speed = m_minSpeed;
        } else if (m_speed > m_maxSpeed) {
            m_speed = m_maxSpeed;
        }
    }

    void
    refreshSolid(void) {
        if (m_colorChanged) {
            for (unsigned idx = 1; idx < LedStripT::SIZE; idx++) {
                m_ledStrip.setPixel(idx, m_rgbColor);
            }
            m_colorChanged = false;
        }
    }

    void
    refreshUi(void) {
        if (m_uiChanged) {
            m_ledStrip.setPixel(m_uiLed, m_uiHsv);
            m_uiChanged = false;
        }
    }

    void
    refreshFlash(void) {
        if (m_counter == m_maxCounter) {
            if (m_flash) {
                for (unsigned idx = 1; idx < LedStripT::SIZE; idx++) {
                    m_ledStrip.setPixel(idx, m_rgbColor);
                }
            } else {
                for (unsigned idx = 1; idx < LedStripT::SIZE; idx++) {
                    m_ledStrip.setPixel(idx, 0);
                }
            }

            m_flash = !m_flash;
        }
    }

    void
    refresh(void) {
        const OutputMode_t outputMode = getCurrentOutputMode();

        refreshUi();

        switch (outputMode) {
        case OutputMode_t::e_Solid:
            refreshSolid();
            break;
        case OutputMode_t::e_Flash:
        default:
            refreshFlash();
            break;
        }
    }

public:
    BlinkenBikeT(LedStripT &p_ledStrip)
      : m_ledStrip(p_ledStrip),
        m_currentState(0),
        m_uiHsv(static_cast<uint16_t>(m_states[m_currentState].m_stateColor), 255, m_maxBrightness / 2),
        m_uiChanged(true),
        m_hsvColor(0, 255, m_maxBrightness),
        m_rgbColor(m_hsvColor),
        m_colorChanged(true),
        m_counter(0),
        m_speed(m_minSpeed),
        m_flash(false)
    {
        static_assert(LedStripT::SIZE > 1, "LED Strip should be more than one LED long!");
        static_assert((LedStripT::SIZE % 2) == 1, "LED Strip should be 1 + an even number of LEDs long!");
        static_assert(m_nStates == 8);

        refresh();
    }

    void
    changeState(void) {
        m_currentState = (m_currentState + 1) % m_nStates;

        m_uiHsv = m_states[m_currentState].m_stateColor;
        m_uiChanged = true;

        refresh();
    }

    void
    blinkTick(void) {
        m_counter -= m_speed;

        if (m_counter < 0) {
            m_counter = m_maxCounter;
        }

        refresh();
    }

    void
    knobTurned(unsigned p_value, bool p_clockwise) {
        const InputMode_t inputMode = getCurrentInputMode();

        m_knob.update(p_value, p_clockwise);

        switch (inputMode) {
        case InputMode_t::e_Color:
            changeColor(m_knob.distance());
            break;
        case InputMode_t::e_Brightness:
            changeBrightness(m_knob.distance());
            break;
        case InputMode_t::e_Speed:
            changeSpeed(m_knob.distance());
            break;
        }
    }
};
/*****************************************************************************/

#endif /* _BLINKENBIKE_HPP_BEF35430_91D6_4F5E_9587_EF88B244E219 */
