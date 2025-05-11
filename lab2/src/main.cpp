#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <numeric>

using namespace std;

// Funkcja obliczająca Cmax dla algorytmu LSA (List Scheduling)
int calculateLSA(int m, const vector<int>& jobs) {
    vector<int> machineLoads(m, 0);  // Obciążenie każdej maszyny (początkowo 0)
    
    for (int p : jobs) {
        // Znajdź maszynę o najmniejszym obciążeniu
        auto min_it = min_element(machineLoads.begin(), machineLoads.end());
        *min_it += p;  // Przydziel zadanie do tej maszyny
    }
    
    return *max_element(machineLoads.begin(), machineLoads.end());  // Cmax
}

// Funkcja obliczająca Cmax dla algorytmu LPT (Longest Processing Time)
int calculateLPT(int m, vector<int> jobs) {
    // Sortowanie zadań malejąco (najdłuższe pierwsze)
    sort(jobs.begin(), jobs.end(), greater<int>());
    return calculateLSA(m, jobs);  // Użyj LSA na posortowanych zadaniach
}

int globalMinCmax = INT_MAX;  // Najlepszy znaleziony Cmax
vector<vector<int>> bestAssignment;  // Najlepszy przydział zadań

// Funkcja rekurencyjna do brute force
void bruteForce(int currentJob, const vector<int>& jobs, vector<vector<int>>& assignment, int m) {
    if (currentJob == int(jobs.size())) {
        // Oblicz Cmax dla bieżącego przydziału
        int currentCmax = 0;
        for (const auto& machine : assignment) {
            int sum = 0;
            for (int task : machine) sum += task;
            if (sum > currentCmax) currentCmax = sum;
        }

        // Aktualizuj najlepsze rozwiązanie
        if (currentCmax < globalMinCmax) {
            globalMinCmax = currentCmax;
            bestAssignment = assignment;
        }
        return;
    }

    // Próbuj przypisać currentJob do każdej maszyny
    for (int i = 0; i < m; ++i) {
        assignment[i].push_back(jobs[currentJob]);  // Przypisz zadanie do maszyny i
        bruteForce(currentJob + 1, jobs, assignment, m);
        assignment[i].pop_back();  // Cofnij przypisanie (backtracking)
    }
}

// Funkcja pomocnicza do wywołania brute force
int findOptimalCmax(int m, const vector<int>& jobs) {
    vector<vector<int>> assignment(m);  // assignment[i] = lista zadań maszyny i
    bruteForce(0, jobs, assignment, m);
    return globalMinCmax;
}

int main() {
    int m = 3;  // Liczba maszyn
    vector<int> jobs = {5, 3, 7, 2, 4, 6};  // Czasy zadań

    int optimalCmax = findOptimalCmax(m, jobs);

    cout << "Optymalny Cmax (brute force): " << optimalCmax << endl;
    cout << "Przydział zadań do maszyn:\n";
    for (int i = 0; i < m; ++i) {
        cout << "Maszyna " << i + 1 << ": ";
        for (int task : bestAssignment[i]) cout << task << " ";
        cout << "(Suma: " << accumulate(bestAssignment[i].begin(), bestAssignment[i].end(), 0) << ")\n";
    }

    return 0;
}