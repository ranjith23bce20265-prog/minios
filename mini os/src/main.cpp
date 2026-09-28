#include "../include/core/Types.hpp"
#include "../include/core/Logger.hpp"
#include "../include/kernel/Kernel.hpp"
#include "../include/kernel/ProcessManager.hpp"
#include "../include/kernel/Scheduler.hpp"
#include "../include/kernel/MemoryManager.hpp"
#include "../include/kernel/VFS.hpp"
#include "../include/kernel/SecurityManager.hpp"
#include "../include/kernel/Interrupts.hpp"
#include "../include/kernel/Deadlock.hpp"
#include "../include/kernel/Sync.hpp"
#include "../include/kernel/DiskScheduler.hpp"

#include <iostream>
#include <string>
#include <sstream>

// Initialize static members for Logger
MiniOS::Logger* MiniOS::Logger::instance = nullptr;
std::mutex MiniOS::Logger::mutex_;

int main() {
    // Clear terminal screen
    std::cout << "\033[2J\033[1;1H";

    // 1. Boot Subsystem (Module 1)
    MiniOS::HardwareSpecs specs;
    specs.totalRamBytes = 1024 * 1024 * 128; // 128 MB RAM
    specs.cpuCores = 8;                      // 8 Cores
    MiniOS::Kernel::getInstance()->bootSystem(specs);

    // 2. Initialize Kernel Subsystems
    MiniOS::ProcessManager procMgr;
    MiniOS::Scheduler scheduler(MiniOS::SchedulingAlgorithm::ROUND_ROBIN, 2);
    MiniOS::MemoryManager memMgr;
    MiniOS::VirtualFileSystem vfs;
    MiniOS::SecurityManager secMgr;
    MiniOS::InterruptManager ivt;

    // Register custom system call interrupt handler (Vector 0x80)
    ivt.registerISR(0x80, []() {
        MiniOS::Logger::getInstance()->log(MiniOS::LogLevel::SUCCESS, "SYSCALL", "Trap 0x80 System Call Execution Handled");
    });

    // Create sample initial processes
    procMgr.createProcess("httpd_server", 6, 1, 1024 * 1024 * 4);
    procMgr.createProcess("database_engine", 8, 2, 1024 * 1024 * 16);

    std::string command;
    std::cout << "\n==================================================\n";
    std::cout << "  Welcome to MiniOS v1.0 Shell!\n";
    std::cout << "  Type 'help' to see all available commands.\n";
    std::cout << "==================================================\n\n";

    // Interactive Command Loop
    while (true) {
        std::cout << "\033[1;32m" << secMgr.getCurrentUser() << "@minios\033[0m:" 
                  << "\033[1;34m" << vfs.getPwd() << "\033[0m$ ";
        
        if (!std::getline(std::cin, command) || command == "shutdown") {
            MiniOS::Logger::getInstance()->log(MiniOS::LogLevel::BOOT, "KERNEL", "Shutting down MiniOS Kernel safely...");
            break;
        }

        if (command == "help") {
            std::cout << "\nAvailable System Commands:\n"
                      << "  ps        - Display Process Table Tree\n"
                      << "  sched     - Run CPU Scheduling Simulation (Round Robin)\n"
                      << "  mem       - Display Physical/Virtual Memory Map\n"
                      << "  translate - Virtual-to-Physical Address Translation Simulation\n"
                      << "  ls        - List Files/Folders in current directory\n"
                      << "  pwd       - Print Working Directory\n"
                      << "  mkdir <d> - Create a new directory (e.g. 'mkdir system')\n"
                      << "  banker    - Run Banker's Deadlock Safety Algorithm\n"
                      << "  disk      - Run SSTF Disk Arm Scheduling\n"
                      << "  trap      - Fire Hardware Interrupt Vector 0x80\n"
                      << "  shutdown  - Exit MiniOS\n\n";
        }
        else if (command == "ps") {
            procMgr.printProcessTree();
        }
        else if (command == "sched") {
            std::vector<std::shared_ptr<MiniOS::PCB>> procs;
            for (auto const& [pid, pcb] : procMgr.getProcessTable()) {
                if (pid != 0) procs.push_back(pcb);
            }
            scheduler.runScheduling(procs);
        }
        else if (command == "mem") {
            memMgr.displayMemoryMap();
        }
        else if (command == "translate") {
            MiniOS::Address phys = memMgr.translateVirtualAddress(1, 0x00001234);
            std::cout << "Translated Virtual 0x00001234 -> Physical 0x" << std::hex << phys << std::dec << "\n";
        }
        else if (command == "ls") {
            vfs.ls();
        }
        else if (command == "pwd") {
            std::cout << vfs.getPwd() << "\n";
        }
        else if (command.rfind("mkdir ", 0) == 0) {
            vfs.mkdir(command.substr(6));
        }
        else if (command == "banker") {
            int n = 5, m = 3;
            std::vector<std::vector<int>> alloc = {{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};
            std::vector<std::vector<int>> max = {{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};
            std::vector<int> avail = {3,3,2};
            MiniOS::BankerAlgorithm::isSafeState(n, m, alloc, max, avail);
        }
        else if (command == "disk") {
            MiniOS::DiskScheduler::SSTF({98, 183, 37, 122, 14, 124, 65, 67}, 53);
        }
        else if (command == "trap") {
            ivt.triggerInterrupt(0x80);
        }
        else if (!command.empty()) {
            std::cout << "Command not recognized. Type 'help' for options.\n";
        }
    }

    return 0;
}