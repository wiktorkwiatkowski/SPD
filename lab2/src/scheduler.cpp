#include "../inc/scheduler.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <climits>
#include <numeric>
#include <queue>
#include <sstream>
#include <iomanip>

void ParallelScheduler::generateRandomInstance(int machines, int tasksCount, int minP, int maxP) {
    machineCount = machines;
    tasks.clear();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(minP, maxP);
    
    for (int i = 0; i < tasksCount; i++) {
        tasks.emplace_back(i+1, dist(gen));
    }
}

void ParallelScheduler::displayInstance() const {
    std::cout << "Machines: " << machineCount << std::endl;
    std::cout << "Tasks:" << std::endl;
    for (const auto& task : tasks) {
        std::cout << "ID: " << task.getId() << " | Time: " << task.getProcessingTime() << std::endl;
    }
    std::cout << "=================================" << std::endl;
}

std::pair<int, long long> ParallelScheduler::calculateLSA() const {
    std::vector<int> machineLoads(machineCount, 0);
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& task : tasks) {
        auto min_it = std::min_element(machineLoads.begin(), machineLoads.end());
        *min_it += task.getProcessingTime();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    int cmax = *std::max_element(machineLoads.begin(), machineLoads.end());
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    return std::make_pair(cmax, time);
}

std::pair<int, long long> ParallelScheduler::calculateLPT() const {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<Task> sortedTasks = tasks;
    std::sort(sortedTasks.begin(), sortedTasks.end(), [](const Task& a, const Task& b) {
        return a.getProcessingTime() > b.getProcessingTime();
    });
    
    std::vector<int> machineLoads(machineCount, 0);
    for (const auto& task : sortedTasks) {
        auto min_it = std::min_element(machineLoads.begin(), machineLoads.end());
        *min_it += task.getProcessingTime();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    int cmax = *std::max_element(machineLoads.begin(), machineLoads.end());
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    return std::make_pair(cmax, time);
}

std::pair<int, long long> ParallelScheduler::bruteForce() const {
    if (tasks.size() > 35) {
        return std::make_pair(-1, -1); // Za duże dla brute force
    }

    int minCmax = INT_MAX;
    auto start = std::chrono::high_resolution_clock::now();
    
    // Prosta implementacja brute force dla 2 maszyn
    int total = 0;
    for (const auto& task : tasks) {
        total += task.getProcessingTime();
    }
    
    int best = total;
    int n = tasks.size();
    
    for (int mask = 0; mask < (1 << n); mask++) {
        int sum1 = 0;
        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) {
                sum1 += tasks[i].getProcessingTime();
            }
        }
        int current = std::max(sum1, total - sum1);
        if (current < best) {
            best = current;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    return std::make_pair(best, time);
}

std::pair<int, long long> ParallelScheduler::dynamicProgramming() const {
    if (machineCount != 2) {
        return std::make_pair(-1, -1); // Tylko dla 2 maszyn
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int sum = 0;
    for (const auto& task : tasks) {
        sum += task.getProcessingTime();
    }
    int target = sum / 2;
    
    std::vector<bool> dp(target + 1, false);
    dp[0] = true;
    
    for (const auto& task : tasks) {
        int p = task.getProcessingTime();
        for (int j = target; j >= p; j--) {
            if (dp[j - p]) {
                dp[j] = true;
            }
        }
    }
    
    int max_possible = 0;
    for (int j = target; j >= 0; j--) {
        if (dp[j]) {
            max_possible = j;
            break;
        }
    }
    
    int cmax = std::max(max_possible, sum - max_possible);
    
    auto end = std::chrono::high_resolution_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    return std::make_pair(cmax, time);
}