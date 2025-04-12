#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <iomanip>

struct Task {
    int p, r, q;
};

void generateInstance(int instance_num, int num_tasks) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Zakresy parametrów (można dostosować)
    std::uniform_int_distribution<int> p_dist(1, 20);  // czas wykonania
    std::uniform_int_distribution<int> r_dist(0, 50);  // czas dostępności
    std::uniform_int_distribution<int> q_dist(1, 30);  // czas stygnięcia

        // Utwórz folder dla danej liczby zadań (jeśli nie istnieje)
        std::string folder_path = "instances/n" + std::to_string(num_tasks);
        system(("mkdir -p " + folder_path).c_str());
    
        // Ścieżka do pliku
        std::string filename = folder_path + "/data" + std::to_string(instance_num) + ".DAT";
        std::ofstream out(filename);
    
    out << num_tasks << "\n";  // Liczba zadań
    
    for (int i = 0; i < num_tasks; ++i) {
        Task t {
            p_dist(gen),
            r_dist(gen),
            q_dist(gen)
        };
        out << t.p << " " << t.r << " " << t.q << "\n";
    }
}

int main() {
    int N, K;
    
    std::cout << "Podaj liczbe zadań w instancji (N): ";
    std::cin >> N;
    std::cout << "Podaj liczbe instancji do wygenerowania (K): ";
    std::cin >> K;

    // Utwórz folder instances jeśli nie istnieje
    system("mkdir -p instances");
    
    // Wygeneruj K instancji
    for (int i = 1; i <= K; ++i) {
        generateInstance(i, N);
        std::cout << "Wygenerowano instancję " << i << "/" << K << " (N = " << N << ")\n";
    }
    
    std::cout << "Gotowe! " << K << " instancji po " << N << " zadań zapisano w folderze instances/\n";
    return 0;
}