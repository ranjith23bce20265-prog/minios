#ifndef MINI_OS_VFS_HPP
#define MINI_OS_VFS_HPP

#include "../core/Logger.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

namespace MiniOS {

    struct FileNode {
        std::string name;
        bool isDirectory;
        std::string content;
        size_t size;
        std::map<std::string, std::shared_ptr<FileNode>> children;
        std::weak_ptr<FileNode> parent;
        std::string permissions = "rw-r--r--";
    };

    class VirtualFileSystem {
    private:
        std::shared_ptr<FileNode> root;
        std::shared_ptr<FileNode> currentDir;

    public:
        VirtualFileSystem() {
            root = std::make_shared<FileNode>();
            root->name = "/";
            root->isDirectory = true;
            root->parent = root;
            currentDir = root;
        }

        bool mkdir(const std::string& dirName) {
            if (currentDir->children.find(dirName) != currentDir->children.end()) return false;
            auto newDir = std::make_shared<FileNode>();
            newDir->name = dirName;
            newDir->isDirectory = true;
            newDir->parent = currentDir;
            currentDir->children[dirName] = newDir;
            return true;
        }

        bool touch(const std::string& fileName, const std::string& content = "") {
            auto file = std::make_shared<FileNode>();
            file->name = fileName;
            file->isDirectory = false;
            file->content = content;
            file->size = content.length();
            file->parent = currentDir;
            currentDir->children[fileName] = file;
            return true;
        }

        void ls() {
            std::cout << "Permissions\tSize\tName\n";
            for (auto const& [name, node] : currentDir->children) {
                std::cout << node->permissions << "\t" << node->size << "B\t" 
                          << (node->isDirectory ? "\033[1;34m" : "\033[0m") << name << "\033[0m\n";
            }
        }

        std::string getPwd() {
            std::string path = "";
            auto curr = currentDir;
            while (curr != curr->parent.lock()) {
                path = "/" + curr->name + path;
                curr = curr->parent.lock();
            }
            return path.empty() ? "/" : path;
        }
    };
}

#endif