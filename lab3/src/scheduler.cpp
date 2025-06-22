#include "../inc/scheduler.h"

#include <algorithm>
#include <chrono>
#include <climits>

#include "../inc/scheduler.h"

int calculateCmax(const FlowShopInstance& instance, const std::vector<int>& perm) {
    int n = static_cast<int>(perm.size());  // Liczba zadań
    int m = instance.num_machines;          // Liczba maszyn

    // Tablica czasów zakończenia
    // C[i][j], i - indeks zadania, j - indeks maszyny
    std::vector<std::vector<int>> C(n, std::vector<int>(m, 0));

    // Pierwsze zadanie, pierwsza maszyna
    C[0][0] = instance.processing_times[perm[0]][0];

    // Pierwsze zadanie, kolejne maszyny
    for (int j = 1; j < m; ++j) C[0][j] = C[0][j - 1] + instance.processing_times[perm[0]][j];

    // Pozostałe zadania
    for (int i = 1; i < n; ++i) {
        // Kolejne zadania
        C[i][0] = C[i - 1][0] + instance.processing_times[perm[i]][0];
        for (int j = 1; j < m; ++j) {
            // Kolejne maszyny
            C[i][j] = std::max(C[i - 1][j], C[i][j - 1]) + instance.processing_times[perm[i]][j];
        }
    }

    return C[n - 1][m - 1];  // Cmax, czyli zakończenie ostatniego zadania na ostaniej maszynie
}

std::pair<int, long long> Scheduler::bruteForce(const FlowShopInstance& instance) {
    using namespace std::chrono;

    int n = instance.num_jobs;
    std::vector<int> perm(n);

    // Początkowa permutacja
    for (int i = 0; i < n; ++i) perm[i] = i;

    int bestCmax = INT_MAX;

    auto start = high_resolution_clock::now();

    do {
        // Liczenie cmax
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

    // Obliczanie sumy czasów dla każdego zadania
    std::vector<std::pair<int, int>> job_times;  // {suma_czasów, id_zadania}
    for (int i = 0; i < n; ++i) {
        int sum = 0;
        for (int j = 0; j < m; ++j) {
            sum += instance.processing_times[i][j];
        }
        job_times.emplace_back(sum, i);
    }

    // Sortowanie zadań malejąco względem sumy czasów
    std::sort(job_times.begin(), job_times.end(), std::greater<>());

    std::vector<int> solution;  // tworzymy permutację
    // Iterujemy po zadaniach, które zostały posortowane w sposób nie rosnący
    // job_id to numer zadania, który chcemy aktualnie wstawic do permutacji
    for (const auto& [_, job_id] : job_times) {
        // Inicjalizacja cmax i odpowiadającej mu permutacji
        int best_cmax = INT_MAX;
        std::vector<int> best_perm;

        // Próbujemy wstawić akutalne zadanie job_id na każde możliwe miejsce w aktualnej
        // permutacji solutions
        for (size_t i = 0; i <= solution.size(); ++i) {
            // Kopia obecnej permutacji
            std::vector<int> candidate = solution;
            candidate.insert(candidate.begin() + i, job_id);

            // Obliczenie cmax dla tego wariantu
            int cmax = calculateCmax(instance, candidate);
            if (cmax < best_cmax) {
                best_cmax = cmax;
                // Najlepsza lokalna permutacja
                best_perm = std::move(candidate);
            }
        }

        solution = std::move(best_perm);  // Najlepsza permutacja
    }

    auto end = high_resolution_clock::now();
    long long time_us = duration_cast<microseconds>(end - start).count();

    return {calculateCmax(instance, solution), time_us};
}

std::pair<int, long long> Scheduler::johnson(const FlowShopInstance& instance) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    if (instance.num_machines != 2) {
        return {-1, 0};  // Działa tylko dla 2 maszyn
    }

    int n = instance.num_jobs;
    std::vector<int> left;   // p1 < p2 (czas przetwarzania na 1 masz. < 2 masz.) idzie na początek
    std::vector<int> right;  // p1 >= p2 (czas przetwarzania na 1 masz. >= 2 masz.) idzie na koniec

    // Podział zadań na left i right
    for (int i = 0; i < n; ++i) {
        int p1 = instance.processing_times[i][0];  // czas na maszynie nr. 1
        int p2 = instance.processing_times[i][1];  // czas na maszynie nr. 2
        if (p1 < p2) {
            left.push_back(i);
        } else {
            right.push_back(i);
        }
    }

    // Sortowanie rosnąco po czasie na pierwszej maszynie
    std::sort(left.begin(), left.end(), [&](int a, int b) {
        return instance.processing_times[a][0] < instance.processing_times[b][0];
    });

    // Sortowanie malejąco po czasie na drugiej maszynie
    std::sort(right.begin(), right.end(), [&](int a, int b) {
        return instance.processing_times[a][1] > instance.processing_times[b][1];
    });

    // Tworzenie permutacji końcowej, połaczenie left i right, doklejanie na koniec
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

    for (int step = 0; step < n; ++step) {
        int job_id = job_times[step].second;
        int best_cmax = INT_MAX;
        std::vector<int> best_order;

        for (size_t pos = 0; pos <= current_order.size(); ++pos) {
            std::vector<int> candidate = current_order;
            candidate.insert(candidate.begin() + pos, job_id);

            // Inkrementalne liczenie Cmax:
            int cmax = 0;
            std::vector<int> machine_times(m, 0);

            for (int i = 0; i < (int)candidate.size(); ++i) {
                int job = candidate[i];
                for (int j = 0; j < m; ++j) {
                    if (i == 0 && j == 0) {
                        machine_times[j] = instance.processing_times[job][j];
                    } else if (j == 0) {
                        machine_times[j] = machine_times[j] + instance.processing_times[job][j];
                    } else {
                        machine_times[j] = std::max(machine_times[j], machine_times[j - 1]) +
                                           instance.processing_times[job][j];
                    }
                }
            }

            cmax = machine_times[m - 1];

            if (cmax < best_cmax) {
                best_cmax = cmax;
                best_order = std::move(candidate);
            }
        }

        current_order = std::move(best_order);
    }

    auto end = high_resolution_clock::now();
    long long time_us = duration_cast<microseconds>(end - start).count();

    return {calculateCmax(instance, current_order), time_us};
}
