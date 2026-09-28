#ifndef MINI_OS_DEADLOCK_HPP
#define MINI_OS_DEADLOCK_HPP

#include <vector>
#include <iostream>

namespace MiniOS {

    class BankerAlgorithm {
    public:
        static bool isSafeState(int n, int m, std::vector<std::vector<int>>& alloc,
                                std::vector<std::vector<int>>& max, std::vector<int>& avail) {
            std::vector<std::vector<int>> need(n, std::vector<int>(m));
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < m; ++j)
                    need[i][j] = max[i][j] - alloc[i][j];

            std::vector<bool> finish(n, false);
            std::vector<int> work = avail;
            std::vector<int> safeSeq;

            for (int k = 0; k < n; ++k) {
                for (int i = 0; i < n; ++i) {
                    if (!finish[i]) {
                        bool possible = true;
                        for (int j = 0; j < m; ++j) {
                            if (need[i][j] > work[j]) { possible = false; break; }
                        }
                        if (possible) {
                            for (int j = 0; j < m; ++j) work[j] += alloc[i][j];
                            finish[i] = true;
                            safeSeq.push_back(i);
                        }
                    }
                }
            }

            if (safeSeq.size() == static_cast<size_t>(n)) {
                std::cout << "\n[DEADLOCK_MGR] System in SAFE State. Sequence: ";
                for (int p : safeSeq) std::cout << "P" << p << " ";
                std::cout << "\n";
                return true;
            }

            std::cout << "\n[DEADLOCK_MGR] DEADLOCK DETECTED / UNSAFE STATE!\n";
            return false;
        }
    };
}

#endif