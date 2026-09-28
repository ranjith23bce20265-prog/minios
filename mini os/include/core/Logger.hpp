// include/core/Logger.hpp
#ifndef MINI_OS_LOGGER_HPP
#define MINI_OS_LOGGER_HPP

#include "Types.hpp"
#include <iostream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <thread>

namespace MiniOS {

    class Logger {
    private:
        static Logger* instance;
        static std::mutex mutex_;
        bool animatedOutput = true;

        Logger() = default;

        std::string getTimestamp() const {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
            
            std::ostringstream oss;
            oss << std::put_time(std::localtime(&time), "%H:%M:%S")
                << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return oss.str();
        }

        std::string getColorCode(LogLevel level) const {
            switch (level) {
                case LogLevel::BOOT:    return "\033[1;35m"; // Bright Magenta
                case LogLevel::INFO:    return "\033[1;36m"; // Bright Cyan
                case LogLevel::SUCCESS: return "\033[1;32m"; // Bright Green
                case LogLevel::WARN:    return "\033[1;33m"; // Bright Yellow
                case LogLevel::ERROR:   return "\033[1;31m"; // Bright Red
                case LogLevel::DEBUG:   return "\033[0;37m"; // Gray
                default:                return "\033[0m";
            }
        }

        std::string getLevelString(LogLevel level) const {
            switch (level) {
                case LogLevel::BOOT:    return "BOOT";
                case LogLevel::INFO:    return "INFO";
                case LogLevel::SUCCESS: return " OK ";
                case LogLevel::WARN:    return "WARN";
                case LogLevel::ERROR:   return "FAIL";
                case LogLevel::DEBUG:   return "DBUG";
                default:                return "LOG ";
            }
        }

    public:
        static Logger* getInstance() {
            std::lock_guard<std::mutex> lock(mutex_);
            if (instance == nullptr) {
                instance = new Logger();
            }
            return instance;
        }

        void log(LogLevel level, const std::string& subsystem, const std::string& message) {
            std::lock_guard<std::mutex> lock(mutex_);
            std::cout << "\033[0;37m[" << getTimestamp() << "]\033[0m "
                      << getColorCode(level) << "[" << getLevelString(level) << "]\033[0m "
                      << "\033[1;34m[" << subsystem << "]\033[0m "
                      << message << std::endl;
        }

        void animateProgressBar(const std::string& task, int durationMs) {
            std::lock_guard<std::mutex> lock(mutex_);
            const int barWidth = 30;
            int steps = 20;
            int delayPerStep = durationMs / steps;

            std::cout << "\033[0;37m[" << getTimestamp() << "]\033[0m "
                      << getColorCode(LogLevel::BOOT) << "[BOOT]\033[0m "
                      << task << " [";

            for (int i = 0; i <= steps; ++i) {
                float progress = (float)i / steps;
                int pos = static_cast<int>(barWidth * progress);
                
                std::cout << "\r\033[0;37m[" << getTimestamp() << "]\033[0m "
                          << getColorCode(LogLevel::BOOT) << "[BOOT]\033[0m "
                          << task << " [";
                
                for (int j = 0; j < barWidth; ++j) {
                    if (j < pos) std::cout << "=";
                    else if (j == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::cout << "] " << int(progress * 100.0) << " %" << std::flush;
                std::this_thread::sleep_for(std::chrono::milliseconds(delayPerStep));
            }
            std::cout << " \033[1;32m[DONE]\033[0m" << std::endl;
        }
    };
}

#endif // MINI_OS_LOGGER_HPP