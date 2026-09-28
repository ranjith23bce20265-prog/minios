// include/kernel/Kernel.hpp
#ifndef MINI_OS_KERNEL_HPP
#define MINI_OS_KERNEL_HPP

#include "../core/Types.hpp"
#include "../core/Logger.hpp"
#include "../hal/DriverManager.hpp"
#include "Bootloader.hpp"
#include <memory>

namespace MiniOS {

    class Kernel {
    private:
        static Kernel* instance;
        SystemState currentState;
        HardwareSpecs specs;
        DriverManager driverManager;

        Kernel();

        void initializeMemorySubsystem();
        void initializeInterruptVectorTable();
        void startProcessScheduler();

    public:
        static Kernel* getInstance();
        
        void bootSystem(const HardwareSpecs& hwSpecs);
        SystemState getState() const { return currentState; }
        const HardwareSpecs& getSpecs() const { return specs; }
    };
}

#endif // MINI_OS_KERNEL_HPP