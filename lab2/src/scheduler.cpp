#include "../inc/scheduler.h"

#include <algorithm>
#include <climits>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <queue>
#include <sstream>

void ParallelScheduler::generateRandomInstance(int machines, int tasksCount, int minP, int maxP) {
    machineCount = machines;
    tasks.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    // Generowanie wartości z rozkładu jednostajnego od minP do maxP
    std::uniform_int_distribution<> dist(minP, maxP);

    // Tworzenie nowego tasku
    for (int i = 0; i < tasksCount; i++) {
        tasks.emplace_back(i + 1, dist(gen));
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
    // Wektor z maszynami, przechowuje łączny czas zadań przypisanych do każdej maszynyn
    // na początek każda maszyna ma 0
    std::vector<int> machineLoads(machineCount, 0);
    // Start pomiaru czasu
    auto start = std::chrono::high_resolution_clock::now();

    // Dla każdego zadania
    for (const auto task : tasks) {
        // Szukanie maszyny z najmniejszym obecnym obciązeniem
        auto min_it = std::min_element(machineLoads.begin(), machineLoads.end());
        // Dodatnie tego czasu do tej maszyny
        *min_it += task.getProcessingTime();
    }

    // Koniec działania algorytmu
    auto end = std::chrono::high_resolution_clock::now();
    // Najdłuższy czas wśród maszyn
    int cmax = *std::max_element(machineLoads.begin(), machineLoads.end());
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return std::make_pair(cmax, time);
}

std::pair<int, long long> ParallelScheduler::calculateLPT() const {
    auto start = std::chrono::high_resolution_clock::now();

    // Kopia wektora
    std::vector<Task> sortedTasks = tasks;

    // Sortowanie od najdłuższego czasu przetwarzania do najkrótszego (malejąco)
    std::sort(sortedTasks.begin(), sortedTasks.end(),
              [](Task& a, Task& b) { return a.getProcessingTime() > b.getProcessingTime(); });

    // Wektor maszyn
    std::vector<int> machineLoads(machineCount, 0);

    // To samo co LSA
    for (const auto& task : sortedTasks) {
        auto min_it = std::min_element(machineLoads.begin(), machineLoads.end());
        *min_it += task.getProcessingTime();
    }

    // Koniec
    auto end = std::chrono::high_resolution_clock::now();
    // Najdłuższy czas wsród wektora maszyn
    int cmax = *std::max_element(machineLoads.begin(), machineLoads.end());
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return std::make_pair(cmax, time);
}

std::pair<int, long long> ParallelScheduler::bruteForce() const {
    if (tasks.size() > 35) {
        return std::make_pair(-1, -1);  // Za duże dla brute force
    }

    int n = tasks.size();
    int bestCmax = INT_MAX;

    auto start = std::chrono::high_resolution_clock::now();

    int combinations = 1 << n;  // 2^n możliwych przypisań

    for (int mask = 0; mask < combinations; ++mask) {
        int sum1 = 0;
        int sum2 = 0;

        for (int i = 0; i < n; ++i) {
            if (mask & (1 << i)) {
                sum1 += tasks[i].getProcessingTime();
            } else {
                sum2 += tasks[i].getProcessingTime();
            }
        }

        int cmax = std::max(sum1, sum2);
        if (cmax < bestCmax) {
            bestCmax = cmax;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return std::pair{bestCmax, time};
}

std::pair<int, long long> ParallelScheduler::dynamicProgramming() const {
    if (machineCount != 2) {
        return std::make_pair(-1, -1);  // tylko dla 2 maszyn
    }

    auto start = std::chrono::high_resolution_clock::now();
    // Liczba zadań
    int taskCount = tasks.size();
    // Całkowity czas wykonania wszystkich zadań
    int sum = 0;
    // Obliczenie sumy wszystkich zadań
    for (const auto& task : tasks) {
        sum += task.getProcessingTime();
    }

    // Maksymalna suma zadań na jednej maszynie
    int target = sum / 2 + 1;  // KL (szkic)

    // Inicjalizacja tablicy T
    std::vector<std::vector<bool>> T(taskCount + 1, std::vector<bool>(target, false));

    // Inicjalizacja pierwszej kolumny (czyli suma 0 możliwa zawsze – "nic nie wybieram")
    for (int taskIndex = 0; taskIndex <= taskCount; taskIndex++) {
        T[taskIndex][0] = true;
    }

    // Wypełnianie tablicy
    for (int taskIndex = 1; taskIndex <= taskCount; taskIndex++) {
        int currentProcessingTime = tasks[taskIndex - 1].getProcessingTime();

        for (int partialSum = 1; partialSum < target; partialSum++) {
            // Jeśli można było osiągnąć sumę partialSum BEZ tego zadania
            // lub można było osiągnąć partialSum - currentProcessingTime i teraz to zadanie
            // dokładamy
            if (T[taskIndex - 1][partialSum] == true || (partialSum >= currentProcessingTime && T[taskIndex - 1][partialSum - currentProcessingTime] == true)) {
                T[taskIndex][partialSum] = true;
            }
        }
    }

    // Szukamy największej możliwej sumy (najbliżej sum/2), którą da się ułożyć
    int bestSubsetSum = 0;

    for (int possibleSum = target - 1; possibleSum >= 0; --possibleSum) {
        if (T[taskCount][possibleSum]) {
            bestSubsetSum = possibleSum;
            break;
        }
    }

    // BACKTRACKING – cofamy się w tablicy T, aby odzyskać konkretne zadania
    std::vector<int> machine1TaskIndices, machine2TaskIndices;
    int machine1TotalTime = 0, machine2TotalTime = 0;

    // Zaczynamy od ostatniego wiersza (czyli wszystkich zadań)
    int currentTaskIndex = taskCount;
    int currentSum = bestSubsetSum;

    // Cofanie się w tablicy i znalezienie zadań, które dały największą sumę
    while (currentTaskIndex > 0) {
        // Pobranie czasu wykonania zadania numer j - 1
        int currentProcessingTime = tasks[currentTaskIndex - 1].getProcessingTime();

        // Sprawdzamy, czy zadanie zostało użyte do zbudowania currentSum
        if (currentSum >= currentProcessingTime &&
            T[currentTaskIndex - 1][currentSum - currentProcessingTime]) {
            // Tak, to zadanie należy do maszyny 1
            machine1TaskIndices.push_back(currentTaskIndex - 1);
            machine1TotalTime += currentProcessingTime;
            currentSum -= currentProcessingTime;
        } else {
            // Nie, to zadanie należy do maszyny 2
            machine2TaskIndices.push_back(currentTaskIndex - 1);
            machine2TotalTime += currentProcessingTime;
        }

        currentTaskIndex--;
    }

    int cmax = std::max(machine1TotalTime, machine2TotalTime);

    auto end = std::chrono::high_resolution_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return std::make_pair(cmax, time);
}

std::pair<int, long long> ParallelScheduler::calculatePTAS(int k) const {
    if (machineCount != 2) return {-1, -1}; // tylko dla 2 maszyn
    if (k > (int)tasks.size()) k = tasks.size();

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<Task> sortedTasks = tasks;
    std::sort(sortedTasks.begin(), sortedTasks.end(), [](const Task& a, const Task& b) {
        return a.getProcessingTime() > b.getProcessingTime();
    });

    int n = sortedTasks.size();
    int bestCmax = INT_MAX;

    // Bruteforce tylko dla pierwszych k zadań
    int combinations = 1 << k;
    std::vector<Task> remainingTasks(sortedTasks.begin() + k, sortedTasks.end());

    for (int mask = 0; mask < combinations; ++mask) {
        int sum1 = 0, sum2 = 0;
        for (int i = 0; i < k; ++i) {
            if (mask & (1 << i))
                sum1 += sortedTasks[i].getProcessingTime();
            else
                sum2 += sortedTasks[i].getProcessingTime();
        }

        std::vector<int> machineLoads = {sum1, sum2};

        // Dodanie pozostałych zadań najpierw dostępnej maszynie
        for (const auto& task : remainingTasks) {
            auto it = std::min_element(machineLoads.begin(), machineLoads.end());
            *it += task.getProcessingTime();
        }

        bestCmax = std::min(bestCmax, std::max(machineLoads[0], machineLoads[1]));
    }

    auto end = std::chrono::high_resolution_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return {bestCmax, time};
}

std::pair<int, long long> ParallelScheduler::calculateFPTAS(double epsilon) const {
    if (machineCount != 2) return {-1, -1};

    auto start = std::chrono::high_resolution_clock::now();

    int n = tasks.size();
    int S = 0;
    for (const auto& task : tasks)
        S += task.getProcessingTime();

    int k = std::max(1, (int)(epsilon * S / (2.0 * n)));

    // Skalowanie czasów
    std::vector<int> scaledProcessingTimes;
    scaledProcessingTimes.reserve(n);
    for (const auto& task : tasks) {
        scaledProcessingTimes.push_back(task.getProcessingTime() / k);
    }

    // Dynamiczne programowanie na przeskalowanych danych
    int target = std::accumulate(scaledProcessingTimes.begin(), scaledProcessingTimes.end(), 0) / 2 + 1;
    std::vector<std::vector<bool>> dp(n + 1, std::vector<bool>(target, false));
    for (int i = 0; i <= n; ++i) dp[i][0] = true;

    for (int i = 1; i <= n; ++i) {
        int p = scaledProcessingTimes[i - 1];
        for (int j = 1; j < target; ++j) {
            dp[i][j] = dp[i - 1][j] || (j >= p && dp[i - 1][j - p]);
        }
    }

    int bestSum = 0;
    for (int j = target - 1; j >= 0; --j) {
        if (dp[n][j]) {
            bestSum = j;
            break;
        }
    }

    // Podział zadań
    std::vector<int> machine1, machine2;
    int i = n, j = bestSum;
    while (i > 0) {
        int p = scaledProcessingTimes[i - 1];
        if (j >= p && dp[i - 1][j - p]) {
            machine1.push_back(i - 1);
            j -= p;
        } else {
            machine2.push_back(i - 1);
        }
        --i;
    }

    int sum1 = 0, sum2 = 0;
    for (int idx : machine1) sum1 += tasks[idx].getProcessingTime();
    for (int idx : machine2) sum2 += tasks[idx].getProcessingTime();

    auto end = std::chrono::high_resolution_clock::now();
    long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    return {std::max(sum1, sum2), time};
}
