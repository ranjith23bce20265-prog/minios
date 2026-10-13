import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.LinkedHashMap;
import java.util.Map;

public class FileManager {
    private static class VirtualFile {
        String name;
        StringBuilder content;
        String createdAt;
        String updatedAt;

        VirtualFile(String name, String initialContent) {
            this.name = name;
            this.content = new StringBuilder(initialContent != null ? initialContent : "");
            String now = nowTimestamp();
            this.createdAt = now;
            this.updatedAt = now;
        }

        int getSize() {
            return content.length();
        }
    }

    private final Map<String, VirtualFile> fileSystem;

    public FileManager() {
        this.fileSystem = new LinkedHashMap<>();
    }

    private static String nowTimestamp() {
        return LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
    }

    public boolean createFile(String fileName, String content) {
        if (fileName == null || fileName.trim().isEmpty()) {
            System.out.println("[FileManager] File name cannot be empty.");
            return false;
        }
        fileName = fileName.trim();
        if (fileSystem.containsKey(fileName)) {
            System.out.println("[FileManager] File '" + fileName + "' already exists.");
            return false;
        }
        fileSystem.put(fileName, new VirtualFile(fileName, content));
        System.out.println("[FileManager] File '" + fileName + "' created successfully.");
        return true;
    }

    public void readFile(String fileName) {
        VirtualFile file = fileSystem.get(fileName);
        if (file == null) {
            System.out.println("[FileManager] File '" + fileName + "' not found.");
            return;
        }
        System.out.println("\n--- File: " + file.name + " (" + file.getSize() + " bytes) ---");
        System.out.println(file.content.length() == 0 ? "<empty file>" : file.content.toString());
        System.out.println("----------------------------------------------");
    }

    public boolean writeFile(String fileName, String newContent, boolean append) {
        VirtualFile file = fileSystem.get(fileName);
        if (file == null) {
            System.out.println("[FileManager] File '" + fileName + "' not found.");
            return false;
        }
        if (!append) {
            file.content.setLength(0);
        } else if (file.content.length() > 0) {
            file.content.append("\n");
        }
        file.content.append(newContent);
        file.updatedAt = nowTimestamp();
        System.out.println("[FileManager] File '" + fileName + "' updated (" + file.getSize() + " bytes).");
        return true;
    }

    public boolean deleteFile(String fileName) {
        VirtualFile removed = fileSystem.remove(fileName);
        if (removed == null) {
            System.out.println("[FileManager] File '" + fileName + "' not found.");
            return false;
        }
        System.out.println("[FileManager] File '" + fileName + "' deleted.");
        return true;
    }

    public void listFiles() {
        if (fileSystem.isEmpty()) {
            System.out.println("\n[FileManager] Virtual directory is empty.");
            return;
        }
        System.out.println("\n+----------------------+------------+---------------------+---------------------+");
        System.out.println("| File Name            | Size (B)   | Created At          | Updated At          |");
        System.out.println("+----------------------+------------+---------------------+---------------------+");
        for (VirtualFile vf : fileSystem.values()) {
            System.out.printf("| %-20s | %-10d | %-19s | %-19s |%n",
                    vf.name, vf.getSize(), vf.createdAt, vf.updatedAt);
        }
        System.out.println("+----------------------+------------+---------------------+---------------------+");
    }

    public int getFileCount() {
        return fileSystem.size();
    }
}
