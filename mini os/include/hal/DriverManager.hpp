// include/hal/DriverManager.hpp
#ifndef MINI_OS_DRIVER_MANAGER_HPP
#define MINI_OS_DRIVER_MANAGER_HPP

#include "../core/Types.hpp"
#include "../core/Logger.hpp"
#include <string>
#include <vector>
#include <memory>

namespace MiniOS {

    class IDriver {
    public:
        virtual ~IDriver() = default;
        virtual std::string getName() const = 0;
        virtual bool initialize() = 0;
        virtual void shutdown() = 0;
    };

    class KeyboardDriver : public IDriver {
    public:
        std::string getName() const override { return "Standard PS/2 & USB Keyboard Driver"; }
        bool initialize() override {
            // Simulate hardware port register check
            return true;
        }
        void shutdown() override {}
    };

    class DisplayDriver : public IDriver {
    public:
        std::string getName() const override { return "VGA/FrameBuffer Display Controller"; }
        bool initialize() override {
            return true;
        }
        void shutdown() override {}
    };

    class StorageDriver : public IDriver {
    public:
        std::string getName() const override { return "AHCI/NVMe Block Device Driver"; }
        bool initialize() override {
            return true;
        }
        void shutdown() override {}
    };

    class DriverManager {
    private:
        std::vector<std::shared_ptr<IDriver>> drivers;

    public:
        void registerDriver(std::shared_ptr<IDriver> driver) {
            drivers.push_back(driver);
        }

        bool loadAllDrivers() {
            auto logger = Logger::getInstance();
            for (auto& driver : drivers) {
                logger->log(LogLevel::INFO, "DRIVER_MGR", "Loading Driver: " + driver->getName());
                std::this_thread::sleep_for(std::chrono::milliseconds(120));
                if (!driver->initialize()) {
                    logger->log(LogLevel::ERROR, "DRIVER_MGR", "Failed to load: " + driver->getName());
                    return false;
                }
                logger->log(LogLevel::SUCCESS, "DRIVER_MGR", "Driver Loaded: " + driver->getName());
            }
            return true;
        }
    };
}

#endif // MINI_OS_DRIVER_MANAGER_HPP