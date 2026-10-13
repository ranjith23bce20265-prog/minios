import java.util.LinkedHashMap;
import java.util.Map;

public class MemoryManager {
    private final int totalMemory; // in KB
    private int usedMemory;
    private final Map<Integer, Integer> allocatedTable; // PID -> Allocated KB

    public MemoryManager(int totalMemory) {
        this.totalMemory = totalMemory;
        this.usedMemory = 0;
        this.allocatedTable = new LinkedHashMap<>();
    }

    public boolean allocateMemory(int pid, int size) {
        if (size <= 0) {
            System.out.println("[MemoryManager] Error: Memory size must be positive.");
            return false;
        }
        if (allocatedTable.containsKey(pid)) {
            System.out.println("[MemoryManager] Process PID " + pid + " already has memory allocated.");
            return false;
        }
        if (usedMemory + size > totalMemory) {
            System.out.println("[MemoryManager] Allocation failed for PID " + pid +
                    ": Requested " + size + " KB, but only " + getFreeMemory() + " KB free.");
            return false;
        }
        allocatedTable.put(pid, size);
        usedMemory += size;
        System.out.println("[MemoryManager] Allocated " + size + " KB to Process PID " + pid + ".");
        return true;
    }

    public boolean deallocateMemory(int pid) {
        Integer size = allocatedTable.remove(pid);
        if (size == null) {
            return false;
        }
        usedMemory -= size;
        System.out.println("[MemoryManager] Freed " + size + " KB from Process PID " + pid + ".");
        return true;
    }

    public int getTotalMemory() {
        return totalMemory;
    }

    public int getUsedMemory() {
        return usedMemory;
    }

    public int getFreeMemory() {
        return totalMemory - usedMemory;
    }

    public void displayMemoryStatus() {
        System.out.println("\n================ MEMORY STATUS ================");
        System.out.println("Total Memory : " + totalMemory + " KB");
        System.out.println("Used Memory  : " + usedMemory + " KB");
        System.out.println("Free Memory  : " + getFreeMemory() + " KB");

        int barLength = 30;
        int filled = (int) Math.round(((double) usedMemory / totalMemory) * barLength);
        StringBuilder bar = new StringBuilder("[");
        for (int i = 0; i < barLength; i++) {
            bar.append(i < filled ? "#" : "-");
        }
        bar.append("] ").append(String.format("%.1f%%", (usedMemory * 100.0) / totalMemory));
        System.out.println("Usage        : " + bar);

        if (allocatedTable.isEmpty()) {
            System.out.println("No active process memory allocations.");
        } else {
            System.out.println("\n+--------+------------------+");
            System.out.println("| PID    | Allocated (KB)   |");
            System.out.println("+--------+------------------+");
            for (Map.Entry<Integer, Integer> entry : allocatedTable.entrySet()) {
                System.out.printf("| %-6d | %-16d |%n", entry.getKey(), entry.getValue());
            }
            System.out.println("+--------+------------------+");
        }
        System.out.println("===============================================");
    }
}
