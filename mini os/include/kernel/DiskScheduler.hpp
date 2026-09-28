#ifndef MINI_OS_DISK_SCHEDULER_HPP
#define MINI_OS_DISK_SCHEDULER_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace MiniOS {

    class DiskScheduler {
    public:
        static void SSTF(std::vector<int> requests, int head) {
            int totalSeek = 0;
            std::cout << "\n[DISK] SSTF Seek Traversal: " << head;

            while (!requests.empty()) {
                auto closestIt = std::min_element(requests.begin(), requests.end(), [head](int a, int b) {
                    return std::abs(a - head) < std::abs(b - head);
                });

                totalSeek += std::abs(*closestIt - head);
                head = *closestIt;
                std::cout << " -> " << head;
                requests.erase(closestIt);
            }

            std::cout << "\nTotal Cylinder Seek Distance: " << totalSeek << " cylinders\n";
        }
    };
}

#endif