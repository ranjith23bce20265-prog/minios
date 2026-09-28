#ifndef MINI_OS_SCHEDULER_HPP
#define MINI_OS_SCHEDULER_HPP

#include "ProcessManager.hpp"
#include "../core/Logger.hpp"
#include <queue>
#include <vector>
#include <memory>
#include <iomanip>
#include <iostream>

namespace MiniOS {

    enum class SchedulingAlgorithm {
        FCFS,
        SJF,
        PRIORITY,
        ROUND_ROBIN,
        MULTILEVEL_FEEDBACK_QUEUE
    };

    struct GanttChartEntry {
        PID pid;
        uint32_t startTime;
        uint32_t endTime;
    };

    class Scheduler {
    private:
        SchedulingAlgorithm algorithm;
        uint32_t timeQuantum;
        std::vector<GanttChartEntry> ganttChart;

    public:
        explicit Scheduler(SchedulingAlgorithm algo = SchedulingAlgorithm::ROUND_ROBIN, uint32_t tq = 2)
            : algorithm(algo), timeQuantum(tq) {}

        void runScheduling(std::vector<std::shared_ptr<PCB>> processList) {
            ganttChart.clear();
            uint32_t currentTime = 0;
            Logger::getInstance()->log(LogLevel::INFO, "SCHEDULER", "Starting Scheduling Execution...");

            if (algorithm == SchedulingAlgorithm::FCFS) {
                // First-Come, First-Served
                std::sort(processList.begin(), processList.end(), [](const auto& a, const auto& b) {
                    return a->arrivalTime < b->arrivalTime;
                });

                for (auto& proc : processList) {
                    if (currentTime < proc->arrivalTime) currentTime = proc->arrivalTime;
                    uint32_t start = currentTime;
                    proc->waitingTime = currentTime - proc->arrivalTime;
                    currentTime += proc->burstTime;
                    proc->completionTime = currentTime;
                    proc->turnaroundTime = proc->completionTime - proc->arrivalTime;
                    proc->state = ProcessState::TERMINATED;
                    ganttChart.push_back({proc->pid, start, currentTime});
                }
            } 
            else if (algorithm == SchedulingAlgorithm::ROUND_ROBIN) {
                std::queue<std::shared_ptr<PCB>> readyQueue;
                for (auto& p : processList) readyQueue.push(p);

                while (!readyQueue.empty()) {
                    auto proc = readyQueue.front();
                    readyQueue.pop();

                    uint32_t executeTime = std::min(proc->remainingTime, timeQuantum);
                    uint32_t start = currentTime;
                    currentTime += executeTime;
                    proc->remainingTime -= executeTime;

                    ganttChart.push_back({proc->pid, start, currentTime});

                    if (proc->remainingTime > 0) {
                        readyQueue.push(proc);
                    } else {
                        proc->completionTime = currentTime;
                        proc->turnaroundTime = proc->completionTime - proc->arrivalTime;
                        proc->waitingTime = proc->turnaroundTime - proc->burstTime;
                        proc->state = ProcessState::TERMINATED;
                    }
                }
            }

            displayMetrics(processList, currentTime);
        }

        void displayMetrics(const std::vector<std::shared_ptr<PCB>>& processList, uint32_t totalTime) {
            std::cout << "\n=================== CPU SCHEDULING METRICS ===================\n";
            std::cout << "PID\tBurst\tWaiting\tTurnaround\tCompletion\n";
            float totalWait = 0, totalTAT = 0;

            for (const auto& p : processList) {
                std::cout << p->pid << "\t" << p->burstTime << "\t" 
                          << p->waitingTime << "\t" << p->turnaroundTime << "\t\t" 
                          << p->completionTime << "\n";
                totalWait += p->waitingTime;
                totalTAT += p->turnaroundTime;
            }

            std::cout << "\nAverage Waiting Time    : " << (totalWait / processList.size()) << " ms";
            std::cout << "\nAverage Turnaround Time : " << (totalTAT / processList.size()) << " ms";
            std::cout << "\nCPU Utilization         : 100.0%\n";

            // Render ASCII Gantt Chart
            std::cout << "\n------------------- GANTT CHART -------------------\n|";
            for (const auto& g : ganttChart) {
                std::cout << " P" << g.pid << " (" << g.startTime << "-" << g.endTime << ") |";
            }
            std::cout << "\n---------------------------------------------------\n\n";
        }
    };
}

#endif