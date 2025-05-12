#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include <utility>
#include <chrono>
#include <random>

// Klasa do reprezentacji jednego zadania
class Task {
    int id; // Identyfikator zadania 
    int processingTime; // czas potrzeby na jego wykonanie
    
public:
    Task(int id, int p) : id(id), processingTime(p) {}
    
    // Zwraca ID
    int getId() const { return id; }

    // Zwraca czas wykonywania 
    int getProcessingTime() const { return processingTime; }
};

// Klasa do planująca zadania
class ParallelScheduler {
    // Liczba maszyn
    int machineCount;
    // Lista zadań, które trzeba rozdzielić pomiędzy maszyny 
    std::vector<Task> tasks;
    
public:
    // Funkcja do generowania losowej instancji zadania
    void generateRandomInstance(int machines, int tasksCount, int minP, int maxP);
    // Wyświetla wygenerowaną instację
    void displayInstance() const;
    
    // 
    std::pair<int, long long> calculateLSA() const;
    std::pair<int, long long> calculateLPT() const;
    std::pair<int, long long> bruteForce() const;
    std::pair<int, long long> dynamicProgramming() const;
    std::pair<int, long long> calculatePTAS(int k) const;
    std::pair<int, long long> calculateFPTAS(double epsilon) const;
    
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