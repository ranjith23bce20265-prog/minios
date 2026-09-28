// include/core/Types.hpp
#ifndef MINI_OS_TYPES_HPP
#define MINI_OS_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace MiniOS {

    using PID = int32_t;
    using Address = uint64_t;
    using Byte = uint8_t;

    enum class SystemState {
        POWER_OFF,
        POST,
        BOOTLOADING,
        KERNEL_INITIALIZATION,
        DRIVER_LOADING,
        MEMORY_INIT,
        SCHEDULER_STARTING,
        SYSTEM_READY,
        HALTED,
        CRASHED
    };

    enum class LogLevel {
        DEBUG,
        INFO,
        SUCCESS,
        WARN,
        ERROR,
        BOOT
    };

    struct HardwareSpecs {
        size_t totalRamBytes = 1024 * 1024 * 64; // 64 MB Virtual RAM
        uint32_t cpuCores = 4;
        uint32_t clockSpeedMHz = 3200;
        size_t pageSize = 4096; // 4KB pages
    };
}

#endif // MINI_OS_TYPES_HPP