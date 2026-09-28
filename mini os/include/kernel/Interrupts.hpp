#ifndef MINI_OS_INTERRUPTS_HPP
#define MINI_OS_INTERRUPTS_HPP

#include "../core/Logger.hpp"
#include <functional>
#include <vector>

namespace MiniOS {

    using ISR = std::function<void()>;

    class InterruptManager {
    private:
        ISR ivt[256];

    public:
        InterruptManager() {
            for (int i = 0; i < 256; ++i) {
                ivt[i] = [i]() {
                    Logger::getInstance()->log(LogLevel::WARN, "IVT", "Unhandled Interrupt Vector: " + std::to_string(i));
                };
            }
        }

        void registerISR(uint8_t vector, ISR isr) {
            ivt[vector] = isr;
        }

        void triggerInterrupt(uint8_t vector) {
            Logger::getInstance()->log(LogLevel::BOOT, "INTERRUPT", "Hardware/Software Trap Vector: 0x" + std::to_string(vector));
            ivt[vector]();
        }
    };
}

#endif