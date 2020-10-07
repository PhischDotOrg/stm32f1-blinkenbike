/*-
 * $Copyright$
 */

#ifndef _SHUTDOWN_HANDLER_HPP_0FE5EDE6_405C_476A_BA52_57B344FE14E8
#define _SHUTDOWN_HANDLER_HPP_0FE5EDE6_405C_476A_BA52_57B344FE14E8

#include <tasks/Task.hpp>

/*****************************************************************************/
namespace tasks {
/*****************************************************************************/

template<
    typename PwrT,
    typename CallbackT
>
class ShutdownHandler : public tasks::Task {
    static constexpr eNotifyAction  m_notifyAction   = eSetValueWithOverwrite;
    const PwrT &                    m_pwr;
    CallbackT &                     m_callback;
    const uint8_t                   m_waitInSeconds;

public:
    enum class Shutdown_e : uint32_t {
        e_Start = 0b01,
        e_Stop  = 0b10
    };

    ShutdownHandler(const char * const p_name, const PwrT &p_pwr, CallbackT & p_callback, uint8_t p_waitInSeconds, const unsigned p_priority = (configMAX_PRIORITIES - 1))
      : tasks::Task(p_name, p_priority), m_pwr(p_pwr), m_callback(p_callback), m_waitInSeconds(p_waitInSeconds) {

    }

    void notify(const Shutdown_e &p_event) {
        xTaskNotify(this->m_handle, static_cast<uint32_t>(p_event), m_notifyAction);
    }

    void notifyIrq(const Shutdown_e &p_event) {
        xTaskNotifyFromISR(this->m_handle, static_cast<uint32_t>(p_event), m_notifyAction, nullptr);
    }

protected:
    void
    run(void) override {
        Shutdown_e event;
        BaseType_t rc;
        TickType_t timeout = portMAX_DELAY;

        do {
            rc = xTaskNotifyWait(0, -1, reinterpret_cast<uint32_t *>(&event), timeout);

            switch (event) {
            case Shutdown_e::e_Start:
                timeout = pdMS_TO_TICKS(m_waitInSeconds * 1000);
                break;
            case Shutdown_e::e_Stop:
                timeout = portMAX_DELAY;
                break;
            }
        } while (rc == pdTRUE);

        m_callback.shutdown();

        taskDISABLE_INTERRUPTS();

        m_pwr.shutdown(PwrT::Mode_t::e_Standby);
    }
};

/*****************************************************************************/
} /* namespace tasks */
/*****************************************************************************/

#endif /* _SHUTDOWN_HANDLER_HPP_0FE5EDE6_405C_476A_BA52_57B344FE14E8 */