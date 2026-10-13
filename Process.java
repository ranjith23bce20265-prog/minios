public class Process {
    private int pid;
    private String name;
    private int arrivalTime;
    private int burstTime;
    private int remainingTime;
    private int priority;
    private int memoryRequired;
    private String state; // NEW, READY, RUNNING, WAITING, TERMINATED
    private int waitingTime;
    private int turnaroundTime;

    public Process(int pid, String name, int arrivalTime, int burstTime, int priority, int memoryRequired) {
        this.pid = pid;
        this.name = name;
        this.arrivalTime = arrivalTime;
        this.burstTime = burstTime;
        this.remainingTime = burstTime;
        this.priority = priority;
        this.memoryRequired = memoryRequired;
        this.state = "READY";
        this.waitingTime = 0;
        this.turnaroundTime = 0;
    }

    public int getPid() {
        return pid;
    }

    public String getName() {
        return name;
    }

    public int getArrivalTime() {
        return arrivalTime;
    }

    public int getBurstTime() {
        return burstTime;
    }

    public int getRemainingTime() {
        return remainingTime;
    }

    public void setRemainingTime(int remainingTime) {
        this.remainingTime = remainingTime;
    }

    public int getPriority() {
        return priority;
    }

    public int getMemoryRequired() {
        return memoryRequired;
    }

    public String getState() {
        return state;
    }

    public void setState(String state) {
        this.state = state;
    }

    public int getWaitingTime() {
        return waitingTime;
    }

    public void setWaitingTime(int waitingTime) {
        this.waitingTime = waitingTime;
    }

    public int getTurnaroundTime() {
        return turnaroundTime;
    }

    public void setTurnaroundTime(int turnaroundTime) {
        this.turnaroundTime = turnaroundTime;
    }

    @Override
    public String toString() {
        return String.format("| %-4d | %-14s | %-7d | %-6d | %-8d | %-9d | %-10s |",
                pid, name, arrivalTime, burstTime, priority, memoryRequired, state);
    }
}
