#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include <utility>
#include <chrono>
#include <random>

class Task {
    int id;
    int processingTime;
    
public:
    Task(int id, int p) : id(id), processingTime(p) {}
    
    int getId() const { return id; }
    int getProcessingTime() const { return processingTime; }
};

class ParallelScheduler {
    int machineCount;
    std::vector<Task> tasks;
    
public:
    void generateRandomInstance(int machines, int tasksCount, int minP, int maxP);
    void displayInstance() const;
    
    std::pair<int, long long> calculateLSA() const;
    std::pair<int, long long> calculateLPT() const;
    std::pair<int, long long> bruteForce() const;
    std::pair<int, long long> dynamicProgramming() const;
    
    const std::vector<Task>& getTasks() const { return tasks; }
    int getMachineCount() const { return machineCount; }
};

class Test {
    ParallelScheduler scheduler;
    
    std::string formatResult(int cmax, int bestCmax, long long time, bool isTimeMs = false) const;
    
public:
    void runTest();
    void saveToCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data);
};

#endif