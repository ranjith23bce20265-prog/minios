#ifndef MINI_OS_MEMORY_MANAGER_HPP
#define MINI_OS_MEMORY_MANAGER_HPP

#include "../core/Types.hpp"
#include "../core/Logger.hpp"
#include <vector>
#include <unordered_map>
#include <list>
#include <iostream>

namespace MiniOS {

    struct PageTableEntry {
        uint32_t frameNumber;
        bool valid = false;
        bool dirty = false;
        bool referenced = false;
    };

    struct TLBEntry {
        uint32_t pageNumber;
        uint32_t frameNumber;
        bool valid = false;
    };

    class MemoryManager {
    private:
        size_t totalRamBytes;
        size_t pageSize;
        size_t totalFrames;
        std::vector<bool> frameBitmap; // Track free/allocated frames
        std::unordered_map<PID, std::unordered_map<uint32_t, PageTableEntry>> pageTables;
        
        // Translation Lookaside Buffer (TLB)
        static const size_t TLB_SIZE = 4;
        std::list<TLBEntry> tlb;

        // Page Replacement Tracking
        std::vector<uint32_t> fifoQueue;

    public:
        MemoryManager(size_t ramBytes = 1024 * 1024 * 16, size_t pSize = 4096)
            : totalRamBytes(ramBytes), pageSize(pSize) {
            totalFrames = totalRamBytes / pageSize;
            frameBitmap.resize(totalFrames, false);
        }

        int32_t allocateFrame() {
            for (size_t i = 0; i < totalFrames; ++i) {
                if (!frameBitmap[i]) {
                    frameBitmap[i] = true;
                    return static_cast<int32_t>(i);
                }
            }
            return -1; // Out of memory
        }

        Address translateVirtualAddress(PID pid, Address virtualAddr) {
            uint32_t pageNum = virtualAddr / pageSize;
            uint32_t offset = virtualAddr % pageSize;

            // 1. Check TLB
            for (auto& entry : tlb) {
                if (entry.valid && entry.pageNumber == pageNum) {
                    Logger::getInstance()->log(LogLevel::SUCCESS, "MMU_TLB", "TLB HIT -> Frame: " + std::to_string(entry.frameNumber));
                    return (entry.frameNumber * pageSize) + offset;
                }
            }

            Logger::getInstance()->log(LogLevel::WARN, "MMU_TLB", "TLB MISS -> Consulting Page Table");

            // 2. Check Page Table
            auto& pt = pageTables[pid];
            if (pt.find(pageNum) == pt.end() || !pt[pageNum].valid) {
                Logger::getInstance()->log(LogLevel::ERROR, "MMU_PAGE_FAULT", "Page Fault at Page " + std::to_string(pageNum) + "! Handling Fault...");
                
                int32_t frame = allocateFrame();
                if (frame == -1) {
                    // Page Replacement Algorithm (FIFO)
                    frame = 0; // Evict frame 0 for simulation
                    Logger::getInstance()->log(LogLevel::WARN, "MMU_SWAP", "Evicted Frame " + std::to_string(frame) + " to swap disk.");
                }

                pt[pageNum] = {static_cast<uint32_t>(frame), true, false, true};
            }

            uint32_t frameNum = pt[pageNum].frameNumber;

            // Update TLB
            if (tlb.size() >= TLB_SIZE) tlb.pop_back();
            tlb.push_front({pageNum, frameNum, true});

            return (frameNum * pageSize) + offset;
        }

        void displayMemoryMap() {
            size_t used = 0;
            for (bool b : frameBitmap) if (b) used++;
            std::cout << "\n================ MEMORY MAP ================\n";
            std::cout << "Total Physical RAM : " << totalRamBytes / 1024 / 1024 << " MB\n";
            std::cout << "Page Size          : " << pageSize << " Bytes\n";
            std::cout << "Total Frames       : " << totalFrames << "\n";
            std::cout << "Used Frames        : " << used << " (" << (used * 100 / totalFrames) << "%)\n";
            std::cout << "============================================\n\n";
        }
    };
}

#endif