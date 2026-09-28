// include/kernel/Bootloader.hpp
#ifndef MINI_OS_BOOTLOADER_HPP
#define MINI_OS_BOOTLOADER_HPP

#include "../core/Types.hpp"
#include "../core/Logger.hpp"
#include <thread>
#include <chrono>

namespace MiniOS {

    class Bootloader {
    private:
        HardwareSpecs specs;

        bool runPOST() {
            auto logger = Logger::getInstance();
            logger->log(LogLevel::BOOT, "BIOS/POST", "Executing Power-On Self Test...");
            
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            logger->log(LogLevel::INFO, "POST", "CPU Core Check: " + std::to_string(specs.cpuCores) + " Cores Detected");
            
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            logger->log(LogLevel::INFO, "POST", "RAM Verification: " + std::to_string(specs.totalRamBytes / (1024 * 1024)) + " MB Virtual RAM OK");
            
            logger->log(LogLevel::SUCCESS, "POST", "Hardware Health Inspection Passed");
            return true;
        }

        bool loadStage1MBR() {
            auto logger = Logger::getInstance();
            logger->log(LogLevel::BOOT, "MBR", "Reading Master Boot Record (Sector 0)...");
            logger->animateProgressBar("Loading MBR Payload", 400);
            logger->log(LogLevel::SUCCESS, "MBR", "Valid Boot Signature [0xAA55] Found");
            return true;
        }

        bool loadStage2KernelImage() {
            auto logger = Logger::getInstance();
            logger->log(LogLevel::BOOT, "GRUB_STAGE2", "Locating Kernel ELF Image at /boot/vmlinuz-minios...");
            logger->animateProgressBar("Decompressing Kernel Image", 600);
            logger->log(LogLevel::SUCCESS, "GRUB_STAGE2", "Kernel Binary Loaded into Virtual Memory at 0x00100000");
            return true;
        }

    public:
        explicit Bootloader(HardwareSpecs hwSpecs) : specs(hwSpecs) {}

        bool executeBootSequence() {
            auto logger = Logger::getInstance();
            logger->log(LogLevel::BOOT, "SYSTEM", "=== INITIATING COLD BOOT SEQUENCE ===");
            
            if (!runPOST()) return false;
            if (!loadStage1MBR()) return false;
            if (!loadStage2KernelImage()) return false;

            return true;
        }
    };
}

#endif // MINI_OS_BOOTLOADER_HPP