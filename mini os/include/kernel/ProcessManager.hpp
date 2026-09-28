#ifndef MINI_OS_PROCESS_MANAGER_HPP
#define MINI_OS_PROCESS_MANAGER_HPP

#include "../core/Types.hpp"
#include "../core/Logger.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <algorithm>

namespace MiniOS {

    enum class ProcessState {
        NEW,
        READY,
        RUNNING,
        WAITING,
        SUSPENDED,
        TERMINATED,
        ZOMBIE
    };

    inline std::string processStateToString(ProcessState state) {
        switch (state) {
            case ProcessState::NEW:        return "NEW";
            case ProcessState::READY:      return "READY";
            case ProcessState::RUNNING:    return "RUNNING";
            case ProcessState::WAITING:    return "WAITING";
            case ProcessState::SUSPENDED:  return "SUSPENDED";
            case ProcessState::TERMINATED: return "TERMINATED";
            case ProcessState::ZOMBIE:     return "ZOMBIE";
            default:                       return "UNKNOWN";
        }
    }

    // Process Control Block (PCB)
    struct PCB {
        PID pid;
        PID parentPid;
        std::string name;
        uint32_t arrivalTime;
        uint32_t burstTime;
        uint32_t remainingTime;
        uint32_t priority;             // 0 = Highest priority
        ProcessState state;
        size_t memoryUsageBytes;
        uint32_t waitingTime{0};
        uint32_t turnaroundTime{0};
        uint32_t completionTime{0};
        std::vector<PID> childPids;

        // Context Switch Register Simulation
        struct Registers {
            uint64_t rax{0}, rbx{0}, rcx{0}, rdx{0};
            uint64_t rip{0x00400000};
            uint64_t rsp{0x007FFF00};
        } registers;
    };

    class ProcessManager {
    private:
        std::unordered_map<PID, std::shared_ptr<PCB>> processTable;
        PID nextPid = 1;

    public:
        ProcessManager() {
            // Create idle process PID 0
            auto idle = std::make_shared<PCB>();
            idle->pid = 0;
            idle->parentPid = -1;
            idle->name = "[system_idle]";
            idle->arrivalTime = 0;
            idle->burstTime = 999999;
            idle->remainingTime = 999999;
            idle->priority = 99;
            idle->state = ProcessState::READY;
            idle->memoryUsageBytes = 4096;
            processTable[0] = idle;
        }

        std::shared_ptr<PCB> createProcess(const std::string& name, uint32_t burstTime, uint32_t priority, size_t memBytes, PID parentPid = 0) {
            auto pcb = std::make_shared<PCB>();
            pcb->pid = nextPid++;
            pcb->parentPid = parentPid;
            pcb->name = name;
            pcb->arrivalTime = 0;
            pcb->burstTime = burstTime;
            pcb->remainingTime = burstTime;
            pcb->priority = priority;
            pcb->state = ProcessState::NEW;
            pcb->memoryUsageBytes = memBytes;

            if (processTable.find(parentPid) != processTable.end()) {
                processTable[parentPid]->childPids.push_back(pcb->pid);
            }

            processTable[pcb->pid] = pcb;
            pcb->state = ProcessState::READY;

            Logger::getInstance()->log(LogLevel::INFO, "PROC_MGR", 
                "Created Process PID " + std::to_string(pcb->pid) + " (" + name + ")");
            return pcb;
        }

        bool killProcess(PID pid) {
            if (processTable.find(pid) == processTable.end() || pid == 0) return false;

            auto pcb = processTable[pid];
            pcb->state = ProcessState::ZOMBIE;

            // Handle Orphan Processes (Re-parent children to PID 1 or PID 0)
            for (PID childPid : pcb->childPids) {
                if (processTable.find(childPid) != processTable.end()) {
                    processTable[childPid]->parentPid = 0; // Adopted by init/idle
                    Logger::getInstance()->log(LogLevel::WARN, "PROC_MGR", 
                        "Orphan Process PID " + std::to_string(childPid) + " adopted by PID 0");
                }
            }

            Logger::getInstance()->log(LogLevel::WARN, "PROC_MGR", "Process PID " + std::to_string(pid) + " terminated (ZOMBIE)");
            return true;
        }

        bool suspendProcess(PID pid) {
            if (processTable.find(pid) != processTable.end()) {
                processTable[pid]->state = ProcessState::SUSPENDED;
                Logger::getInstance()->log(LogLevel::INFO, "PROC_MGR", "Suspended PID " + std::to_string(pid));
                return true;
            }
            return false;
        }

        bool resumeProcess(PID pid) {
            if (processTable.find(pid) != processTable.end() && processTable[pid]->state == ProcessState::SUSPENDED) {
                processTable[pid]->state = ProcessState::READY;
                Logger::getInstance()->log(LogLevel::INFO, "PROC_MGR", "Resumed PID " + std::to_string(pid));
                return true;
            }
            return false;
        }

        void contextSwitch(std::shared_ptr<PCB> oldProc, std::shared_ptr<PCB> newProc) {
            if (oldProc) {
                // Save registers
                oldProc->registers.rip += 4;
                if (oldProc->state == ProcessState::RUNNING) {
                    oldProc->state = ProcessState::READY;
                }
            }
            if (newProc) {
                newProc->state = ProcessState::RUNNING;
                Logger::getInstance()->log(LogLevel::DEBUG, "CPU_CTX", 
                    "Switch: PID " + std::to_string(oldProc ? oldProc->pid : -1) + 
                    " -> PID " + std::to_string(newProc->pid) + " [RIP: 0x" + 
                    std::to_string(newProc->registers.rip) + "]");
            }
        }

        void printProcessTree(PID rootPid = 0, int depth = 0) {
            if (processTable.find(rootPid) == processTable.end()) return;
            auto proc = processTable[rootPid];

            std::string indent(depth * 4, ' ');
            std::cout << indent << "|-- " << proc->name << " (PID: " << proc->pid 
                      << ", State: " << processStateToString(proc->state) 
                      << ", Mem: " << proc->memoryUsageBytes / 1024 << "KB)\n";

            for (PID childPid : proc->childPids) {
                printProcessTree(childPid, depth + 1);
            }
        }

        std::unordered_map<PID, std::shared_ptr<PCB>>& getProcessTable() { return processTable; }
    };
}

#endif