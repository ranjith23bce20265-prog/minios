import java.util.ArrayList;
import java.util.List;

public class ProcessManager {
    private final List<Process> processes;
    private final MemoryManager memoryManager;
    private int nextPid;

    public ProcessManager(MemoryManager memoryManager) {
        this.processes = new ArrayList<>();
        this.memoryManager = memoryManager;
        this.nextPid = 1;
    }

    public Process createProcess(String name, int arrivalTime, int burstTime, int priority, int memoryRequired) {
        int pid = nextPid++;
        if (!memoryManager.allocateMemory(pid, memoryRequired)) {
            System.out.println("[ProcessManager] Failed to create process '" + name + "' due to insufficient memory.");
            return null;
        }
        Process process = new Process(pid, name, arrivalTime, burstTime, priority, memoryRequired);
        processes.add(process);
        System.out.println("[ProcessManager] Created Process '" + name + "' with PID " + pid + ".");
        return process;
    }

    public boolean terminateProcess(int pid) {
        Process process = getProcessById(pid);
        if (process == null) {
            System.out.println("[ProcessManager] Process with PID " + pid + " not found.");
            return false;
        }
        if ("TERMINATED".equals(process.getState())) {
            System.out.println("[ProcessManager] Process PID " + pid + " is already terminated.");
            return false;
        }
        process.setState("TERMINATED");
        process.setRemainingTime(0);
        memoryManager.deallocateMemory(pid);
        System.out.println("[ProcessManager] Process PID " + pid + " (" + process.getName() + ") terminated.");
        return true;
    }

    public Process getProcessById(int pid) {
        for (Process p : processes) {
            if (p.getPid() == pid) {
                return p;
            }
        }
        return null;
    }

    public List<Process> getReadyProcesses() {
        List<Process> ready = new ArrayList<>();
        for (Process p : processes) {
            if ("READY".equals(p.getState())) {
                ready.add(p);
            }
        }
        return ready;
    }

    public List<Process> getAllProcesses() {
        return processes;
    }

    public void listProcesses() {
        if (processes.isEmpty()) {
            System.out.println("\n[ProcessManager] No processes in the system.");
            return;
        }
        System.out.println("\n+------+----------------+---------+--------+----------+-----------+------------+");
        System.out.println("| PID  | Name           | Arrival | Burst  | Priority | Mem (KB)  | State      |");
        System.out.println("+------+----------------+---------+--------+----------+-----------+------------+");
        for (Process p : processes) {
            System.out.println(p);
        }
        System.out.println("+------+----------------+---------+--------+----------+-----------+------------+");
    }
}
