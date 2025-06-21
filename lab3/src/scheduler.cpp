#include "../inc/scheduler.h"
#include <algorithm>
#include <chrono>
#include <climits>

int calculateCmax(const FlowShopInstance& instance, const std::vector<int>& perm) {
    int n = instance.num_jobs;
    int m = instance.num_machines;

    std::vector<std::vector<int>> C(n, std::vector<int>(m, 0));

    // Pierwsze zadanie, pierwsza maszyna
    C[0][0] = instance.processing_times[perm[0]][0];

    // Pierwsze zadanie, kolejne maszyny
    for (int j = 1; j < m; ++j)
        C[0][j] = C[0][j - 1] + instance.processing_times[perm[0]][j];

    // Pozostałe zadania
    for (int i = 1; i < n; ++i) {
        C[i][0] = C[i - 1][0] + instance.processing_times[perm[i]][0];
        for (int j = 1; j < m; ++j) {
            C[i][j] = std::max(C[i - 1][j], C[i][j - 1]) + instance.processing_times[perm[i]][j];
        }
    }

    return C[n - 1][m - 1]; // Cmax
}

std::pair<int, long long> Scheduler::bruteForce(const FlowShopInstance& instance) {
    using namespace std::chrono;

    int n = instance.num_jobs;
    std::vector<int> perm(n);
    for (int i = 0; i < n; ++i)
        perm[i] = i;

    int bestCmax = INT_MAX;

    auto start = high_resolution_clock::now();

    do {
        int cmax = calculateCmax(instance, perm);
        if (cmax < bestCmax) {
            bestCmax = cmax;
        }
    } while (std::next_permutation(perm.begin(), perm.end()));

    auto end = high_resolution_clock::now();
    long long time_us = duration_cast<microseconds>(end - start).count();

    return {bestCmax, time_us};
}
