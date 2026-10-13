import java.util.ArrayList;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

public class Scheduler {
    private final ProcessManager processManager;

    public Scheduler(ProcessManager processManager) {
        this.processManager = processManager;
    }

    // 1. First-Come, First-Served (FCFS)
    public void scheduleFCFS() {
        List<Process> readyList = new ArrayList<>(processManager.getReadyProcesses());
        if (readyList.isEmpty()) {
            System.out.println("[Scheduler] No READY processes available to schedule.");
            return;
        }
        readyList.sort(Comparator.comparingInt(Process::getArrivalTime).thenComparingInt(Process::getPid));
        System.out.println("\n=== Running FCFS (First-Come, First-Served) Scheduler ===");
        runNonPreemptive(readyList);
    }

    // 2. Shortest Job First (SJF - Non-Preemptive)
    public void scheduleSJF() {
        List<Process> readyList = new ArrayList<>(processManager.getReadyProcesses());
        if (readyList.isEmpty()) {
            System.out.println("[Scheduler] No READY processes available to schedule.");
            return;
        }
        readyList.sort(Comparator.comparingInt(Process::getBurstTime).thenComparingInt(Process::getArrivalTime));
        System.out.println("\n=== Running SJF (Shortest Job First) Scheduler ===");
        runNonPreemptive(readyList);
    }

    // 3. Priority Scheduling (Lower number = Higher priority)
    public void schedulePriority() {
        List<Process> readyList = new ArrayList<>(processManager.getReadyProcesses());
        if (readyList.isEmpty()) {
            System.out.println("[Scheduler] No READY processes available to schedule.");
            return;
        }
        readyList.sort(Comparator.comparingInt(Process::getPriority).thenComparingInt(Process::getArrivalTime));
        System.out.println("\n=== Running Priority Scheduler (Lower number = Higher priority) ===");
        runNonPreemptive(readyList);
    }

    // 4. Round Robin (RR)
    public void scheduleRoundRobin(int timeQuantum) {
        if (timeQuantum <= 0) {
            System.out.println("[Scheduler] Time quantum must be greater than 0.");
            return;
        }
        List<Process> readyList = new ArrayList<>(processManager.getReadyProcesses());
        if (readyList.isEmpty()) {
            System.out.println("[Scheduler] No READY processes available to schedule.");
            return;
        }
        readyList.sort(Comparator.comparingInt(Process::getArrivalTime).thenComparingInt(Process::getPid));
        System.out.println("\n=== Running Round Robin Scheduler (Quantum = " + timeQuantum + ") ===");

        for (Process p : readyList) {
            p.setRemainingTime(p.getBurstTime());
        }

        Queue<Process> queue = new LinkedList<>(readyList);
        int currentTime = 0;
        StringBuilder gantt = new StringBuilder("|");

        while (!queue.isEmpty()) {
            Process current = queue.poll();
            current.setState("RUNNING");

            int execTime = Math.min(timeQuantum, current.getRemainingTime());
            gantt.append(" P").append(current.getPid()).append(" (").append(currentTime)
                 .append("-").append(currentTime + execTime).append(") |");

            currentTime += execTime;
            current.setRemainingTime(current.getRemainingTime() - execTime);

            if (current.getRemainingTime() > 0) {
                current.setState("READY");
                queue.offer(current);
            } else {
                int turnaround = Math.max(currentTime - current.getArrivalTime(), current.getBurstTime());
                int waiting = Math.max(turnaround - current.getBurstTime(), 0);
                current.setTurnaroundTime(turnaround);
                current.setWaitingTime(waiting);
                processManager.terminateProcess(current.getPid());
            }
        }

        printResults(readyList, gantt.toString());
    }

    private void runNonPreemptive(List<Process> orderedProcesses) {
        int currentTime = 0;
        StringBuilder gantt = new StringBuilder("|");

        for (Process p : orderedProcesses) {
            if (currentTime < p.getArrivalTime()) {
                currentTime = p.getArrivalTime();
            }
            p.setState("RUNNING");
            int start = currentTime;
            currentTime += p.getBurstTime();

            p.setWaitingTime(start - p.getArrivalTime());
            p.setTurnaroundTime(currentTime - p.getArrivalTime());

            gantt.append(" P").append(p.getPid()).append(" (").append(start)
                 .append("-").append(currentTime).append(") |");

            processManager.terminateProcess(p.getPid());
        }

        printResults(orderedProcesses, gantt.toString());
    }

    private void printResults(List<Process> processes, String ganttChart) {
        System.out.println("\nGantt Chart:");
        System.out.println(ganttChart);

        System.out.println("\n+------+----------------+--------+---------+------------+");
        System.out.println("| PID  | Name           | Burst  | Waiting | Turnaround |");
        System.out.println("+------+----------------+--------+---------+------------+");

        double totalWT = 0;
        double totalTAT = 0;
        for (Process p : processes) {
            totalWT += p.getWaitingTime();
            totalTAT += p.getTurnaroundTime();
            System.out.printf("| %-4d | %-14s | %-6d | %-7d | %-10d |%n",
                    p.getPid(), p.getName(), p.getBurstTime(), p.getWaitingTime(), p.getTurnaroundTime());
        }
        System.out.println("+------+----------------+--------+---------+------------+");
        System.out.printf("Average Waiting Time    : %.2f units%n", totalWT / processes.size());
        System.out.printf("Average Turnaround Time : %.2f units%n", totalTAT / processes.size());
    }
}
