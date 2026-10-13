import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        MemoryManager memoryManager = new MemoryManager(1024); // 1024 KB total RAM
        ProcessManager processManager = new ProcessManager(memoryManager);
        Scheduler scheduler = new Scheduler(processManager);
        FileManager fileManager = new FileManager();

        System.out.println("==================================================");
        System.out.println("          Welcome to MiniOS Simulator             ");
        System.out.println("==================================================");

        boolean running = true;
        while (running) {
            System.out.println("\n------------------- MAIN MENU --------------------");
            System.out.println("1. Create Process");
            System.out.println("2. List Processes");
            System.out.println("3. Terminate Process");
            System.out.println("4. Run CPU Scheduler (FCFS / SJF / Priority / RR)");
            System.out.println("5. Display Memory Status");
            System.out.println("6. File Manager Operations");
            System.out.println("7. Load Sample Demo Data");
            System.out.println("0. Shutdown MiniOS");
            System.out.print("Select an option: ");

            int choice = readInt(scanner);
            switch (choice) {
                case 1:
                    System.out.print("Enter Process Name: ");
                    String name = scanner.nextLine().trim();
                    if (name.isEmpty()) name = "Proc";
                    System.out.print("Enter Arrival Time (e.g., 0): ");
                    int arrival = readInt(scanner);
                    System.out.print("Enter Burst Time (e.g., 5): ");
                    int burst = readInt(scanner);
                    System.out.print("Enter Priority (1 = highest): ");
                    int priority = readInt(scanner);
                    System.out.print("Enter Memory Required in KB (e.g., 128): ");
                    int memory = readInt(scanner);

                    processManager.createProcess(name, Math.max(0, arrival), Math.max(1, burst), priority, memory);
                    break;

                case 2:
                    processManager.listProcesses();
                    break;

                case 3:
                    System.out.print("Enter PID to terminate: ");
                    int pid = readInt(scanner);
                    processManager.terminateProcess(pid);
                    break;

                case 4:
                    handleSchedulerMenu(scanner, scheduler);
                    break;

                case 5:
                    memoryManager.displayMemoryStatus();
                    break;

                case 6:
                    handleFileMenu(scanner, fileManager);
                    break;

                case 7:
                    loadDemoData(processManager, fileManager);
                    break;

                case 0:
                    running = false;
                    System.out.println("Shutting down MiniOS... Goodbye!");
                    break;

                default:
                    System.out.println("Invalid option. Please try again.");
            }
        }
        scanner.close();
    }

    private static void handleSchedulerMenu(Scanner scanner, Scheduler scheduler) {
        System.out.println("\n--- CPU Scheduler Algorithms ---");
        System.out.println("1. First-Come, First-Served (FCFS)");
        System.out.println("2. Shortest Job First (SJF)");
        System.out.println("3. Priority Scheduling");
        System.out.println("4. Round Robin (RR)");
        System.out.print("Choose algorithm: ");

        int algo = readInt(scanner);
        switch (algo) {
            case 1:
                scheduler.scheduleFCFS();
                break;
            case 2:
                scheduler.scheduleSJF();
                break;
            case 3:
                scheduler.schedulePriority();
                break;
            case 4:
                System.out.print("Enter Time Quantum: ");
                int quantum = readInt(scanner);
                scheduler.scheduleRoundRobin(quantum);
                break;
            default:
                System.out.println("Invalid scheduler choice.");
        }
    }

    private static void handleFileMenu(Scanner scanner, FileManager fileManager) {
        System.out.println("\n--- File Manager ---");
        System.out.println("1. Create File");
        System.out.println("2. Read File");
        System.out.println("3. Write (Overwrite) File");
        System.out.println("4. Append to File");
        System.out.println("5. Delete File");
        System.out.println("6. List All Files");
        System.out.print("Choose file operation: ");

        int op = readInt(scanner);
        switch (op) {
            case 1:
                System.out.print("Enter file name: ");
                String createName = scanner.nextLine().trim();
                System.out.print("Enter initial content: ");
                String content = scanner.nextLine();
                fileManager.createFile(createName, content);
                break;
            case 2:
                System.out.print("Enter file name to read: ");
                fileManager.readFile(scanner.nextLine().trim());
                break;
            case 3:
                System.out.print("Enter file name to overwrite: ");
                String writeName = scanner.nextLine().trim();
                System.out.print("Enter new content: ");
                String newContent = scanner.nextLine();
                fileManager.writeFile(writeName, newContent, false);
                break;
            case 4:
                System.out.print("Enter file name to append: ");
                String appendName = scanner.nextLine().trim();
                System.out.print("Enter content to append: ");
                String addContent = scanner.nextLine();
                fileManager.writeFile(appendName, addContent, true);
                break;
            case 5:
                System.out.print("Enter file name to delete: ");
                fileManager.deleteFile(scanner.nextLine().trim());
                break;
            case 6:
                fileManager.listFiles();
                break;
            default:
                System.out.println("Invalid file operation.");
        }
    }

    private static void loadDemoData(ProcessManager pm, FileManager fm) {
        System.out.println("\nLoading sample processes and files...");
        pm.createProcess("SystemInit", 0, 6, 1, 128);
        pm.createProcess("Browser", 1, 8, 3, 256);
        pm.createProcess("TextEditor", 2, 4, 2, 64);
        pm.createProcess("MusicPlayer", 3, 5, 4, 128);

        fm.createFile("readme.txt", "Welcome to MiniOS Virtual File System!");
        fm.createFile("syslog.log", "Boot sequence completed successfully.");
    }

    private static int readInt(Scanner scanner) {
        while (true) {
            String line = scanner.nextLine().trim();
            try {
                return Integer.parseInt(line);
            } catch (NumberFormatException e) {
                System.out.print("Please enter a valid number: ");
            }
        }
    }
}
