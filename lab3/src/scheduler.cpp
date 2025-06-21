#include "../inc/scheduler.h"

#include <algorithm>
#include <chrono>
#include <climits>

#include "../inc/scheduler.h"

int calculateCmax(const FlowShopInstance& instance, const std::vector<int>& perm) {
    int n = static_cast<int>(perm.size());
    int m = instance.num_machines;

    std::vector<std::vector<int>> C(n, std::vector<int>(m, 0));

    // Pierwsze zadanie, pierwsza maszyna
    C[0][0] = instance.processing_times[perm[0]][0];

    // Pierwsze zadanie, kolejne maszyny
    for (int j = 1; j < m; ++j) C[0][j] = C[0][j - 1] + instance.processing_times[perm[0]][j];

    // Pozostałe zadania
    for (int i = 1; i < n; ++i) {
        C[i][0] = C[i - 1][0] + instance.processing_times[perm[i]][0];
        for (int j = 1; j < m; ++j) {
            C[i][j] = std::max(C[i - 1][j], C[i][j - 1]) + instance.processing_times[perm[i]][j];
        }
    }

    return C[n - 1][m - 1];  // Cmax
}

std::pair<int, long long> Scheduler::bruteForce(const FlowShopInstance& instance) {
    using namespace std::chrono;

    int n = instance.num_jobs;
    std::vector<int> perm(n);
    for (int i = 0; i < n; ++i) perm[i] = i;

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

std::pair<int, long long> Scheduler::calculateNEH(const FlowShopInstance& instance) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    int n = instance.num_jobs;
    int m = instance.num_machines;

    // 1. Oblicz sumę czasów operacji dla każdego zadania
    std::vector<std::pair<int, int>> job_times;  // {suma_czasów, id_zadania}
    for (int i = 0; i < n; ++i) {
        int sum = 0;
        for (int j = 0; j < m; ++j) {
            sum += instance.processing_times[i][j];
        }
        job_times.emplace_back(sum, i);
    }

    // 2. Posortuj zadania malejąco względem sumy czasów
    std::sort(job_times.begin(), job_times.end(), std::greater<>());

    std::vector<int> solution;  // π*
    for (const auto& [_, job_id] : job_times) {
        int best_cmax = INT_MAX;
        std::vector<int> best_perm;

        // 4. Wstaw zadanie job_id na każdą możliwą pozycję w π*
        for (size_t i = 0; i <= solution.size(); ++i) {
            std::vector<int> candidate = solution;
            candidate.insert(candidate.begin() + i, job_id);

            int cmax = calculateCmax(instance, candidate);
            if (cmax < best_cmax) {
                best_cmax = cmax;
                best_perm = std::move(candidate);
            }
        }

        solution = std::move(best_perm);  // zaakceptuj najlepszą permutację
    }

    auto end = high_resolution_clock::now();
    long long time_us = duration_cast<microseconds>(end - start).count();

    return {calculateCmax(instance, solution), time_us};
}

std::pair<int, long long> Scheduler::johnson(const FlowShopInstance& instance) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    if (instance.num_machines != 2) {
        return {-1, 0};  // Johnson działa tylko dla 2 maszyn
    }

    int n = instance.num_jobs;
    std::vector<int> left;   // J_L: p1 < p2
    std::vector<int> right;  // J_R: p1 >= p2

    // Podział zadań na J_L i J_R
    for (int i = 0; i < n; ++i) {
        int p1 = instance.processing_times[i][0];
        int p2 = instance.processing_times[i][1];
        if (p1 < p2) {
            left.push_back(i);
        } else {
            right.push_back(i);
        }
    }

    // Sortowanie J_L rosnąco względem p1
    std::sort(left.begin(), left.end(), [&](int a, int b) {
        return instance.processing_times[a][0] < instance.processing_times[b][0];
    });

    // Sortowanie J_R malejąco względem p2
    std::sort(right.begin(), right.end(), [&](int a, int b) {
        return instance.processing_times[a][1] > instance.processing_times[b][1];
    });

    // Połączenie kolejności: π* = π_L ∪ π_R
    std::vector<int> permutation;
    permutation.insert(permutation.end(), left.begin(), left.end());
    permutation.insert(permutation.end(), right.begin(), right.end());

    int cmax = calculateCmax(instance, permutation);
    auto end = high_resolution_clock::now();
    long long time_us = duration_cast<microseconds>(end - start).count();

    return {cmax, time_us};
}

std::pair<int, long long> Scheduler::calculateFNEH(const FlowShopInstance& instance) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    int n = instance.num_jobs;
    int m = instance.num_machines;

    // Oblicz sumy czasów przetwarzania
    std::vector<std::pair<int, int>> job_times;
    for (int i = 0; i < n; ++i) {
        int sum = 0;
        for (int j = 0; j < m; ++j) {
            sum += instance.processing_times[i][j];
        }
        job_times.emplace_back(sum, i);
    }

    std::sort(job_times.begin(), job_times.end(), std::greater<>());

    std::vector<int> current_order;
    std::vector<std::vector<int>> completion(n, std::vector<int>(m, 0));

    for (int step = 0; step < n; ++step) {
        int job_id = job_times[step].second;
        int best_cmax = INT_MAX;
        std::vector<int> best_order;

        for (size_t pos = 0; pos <= current_order.size(); ++pos) {
            std::vector<int> temp_order = current_order;
            temp_order.insert(temp_order.begin() + pos, job_id);

            int cmax = calculateCmax(instance, temp_order);  // Można przyspieszyć cache’ując C
            if (cmax < best_cmax) {
                best_cmax = cmax;
                best_order = std::move(temp_order);
            }
        }

        current_order = std::move(best_order);
    }

    int final_cmax = calculateCmax(instance, current_order);

    auto end = high_resolution_clock::now();
    long long time_us = duration_cast<microseconds>(end - start).count();

    return {final_cmax, time_us};
}
