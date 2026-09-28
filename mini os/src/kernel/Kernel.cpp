// src/kernel/Kernel.cpp
#include "../../include/kernel/Kernel.hpp"
#include <thread>
#include <chrono>

namespace MiniOS {

    Kernel* Kernel::instance = nullptr;

    Kernel::Kernel() : currentState(SystemState::POWER_OFF) {}

    Kernel* Kernel::getInstance() {
        if (instance == nullptr) {
            instance = new Kernel();
        }
        return instance;
    }

    void Kernel::initializeMemorySubsystem() {
        auto logger = Logger::getInstance();
        currentState = SystemState::MEMORY_INIT;
        logger->log(LogLevel::BOOT, "KERNEL", "Initializing Virtual Memory Manager...");
        
        logger->animateProgressBar("Allocating Page Tables", 500);
        logger->log(LogLevel::INFO, "MMU", "Identity Mapping 0x0 - 0x04000000 (64MB)");
        logger->log(LogLevel::SUCCESS, "MMU", "Paging Enabled (4KB Page Size, 2-Level Page Table Structure)");
    }

    void Kernel::initializeInterruptVectorTable() {
        auto logger = Logger::getInstance();
        logger->log(LogLevel::BOOT, "KERNEL", "Populating Interrupt Vector Table (IVT)...");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        
        logger->log(LogLevel::INFO, "IVT", "Vector 0x00: Division By Zero Exception Handler Registered");
        logger->log(LogLevel::INFO, "IVT", "Vector 0x0E: Page Fault Exception Handler Registered");
        logger->log(LogLevel::INFO, "IVT", "Vector 0x20: System Timer Hardware Interrupt IRQ0 Registered");
        logger->log(LogLevel::INFO, "IVT", "Vector 0x80: System Call (Syscall) Trap Registered");
        logger->log(LogLevel::SUCCESS, "IVT", "IVT Initialization Complete (256 Entries Configured)");
    }

    void Kernel::startProcessScheduler() {
        auto logger = Logger::getInstance();
        currentState = SystemState::SCHEDULER_STARTING;
        logger->log(LogLevel::BOOT, "KERNEL", "Bootstrapping Multi-Tasking Process Scheduler...");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        logger->log(LogLevel::INFO, "SCHEDULER", "Spawning Process Control Block (PCB) for PID 0 [swapper/idle]");
        logger->log(LogLevel::INFO, "SCHEDULER", "Spawning Process Control Block (PCB) for PID 1 [/sbin/init]");
        logger->log(LogLevel::SUCCESS, "SCHEDULER", "Preemptive Pre-configured Scheduler Running");
    }

    void Kernel::bootSystem(const HardwareSpecs& hwSpecs) {
        this->specs = hwSpecs;
        auto logger = Logger::getInstance();

        // 1. Bootloader Execution
        currentState = SystemState::BOOTLOADING;
        Bootloader bootloader(specs);
        if (!bootloader.executeBootSequence()) {
            currentState = SystemState::CRASHED;
            logger->log(LogLevel::ERROR, "KERNEL", "Kernel Initialization Aborted Due to Bootloader Error!");
            return;
        }

        // 2. Kernel Subsystems Boostrap
        currentState = SystemState::KERNEL_INITIALIZATION;
        logger->log(LogLevel::BOOT, "KERNEL", "Kernel Execution Handover Successful. Booting Subsystems...");

        initializeMemorySubsystem();
        initializeInterruptVectorTable();

        // 3. Driver Registration & Loading
        currentState = SystemState::DRIVER_LOADING;
        logger->log(LogLevel::BOOT, "KERNEL", "Initializing Device Drivers...");
        
        driverManager.registerDriver(std::make_shared<DisplayDriver>());
        driverManager.registerDriver(std::make_shared<KeyboardDriver>());
        driverManager.registerDriver(std::make_shared<StorageDriver>());

        if (!driverManager.loadAllDrivers()) {
            currentState = SystemState::CRASHED;
            logger->log(LogLevel::ERROR, "KERNEL", "Driver Loading Failed!");
            return;
        }

        // 4. Start Scheduler & System Ready State
        startProcessScheduler();

        currentState = SystemState::SYSTEM_READY;
        logger->log(LogLevel::BOOT, "KERNEL", "==================================================");
        logger->log(LogLevel::SUCCESS, "KERNEL", "MINI-OS KERNEL INITIALIZED SUCCESSFULLY");
        logger->log(LogLevel::SUCCESS, "KERNEL", "SYSTEM TRANSITIONED TO READY STATE");
        logger->log(LogLevel::BOOT, "KERNEL", "==================================================");
      }
}