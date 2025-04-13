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

    // Skalowanie zakresów w zależności od liczby zadań
    int p_min, p_max, q_min, q_max, r_spread;
    if (num_tasks <= 10) {
        p_min = 1; p_max = 30;
        q_min = 1; q_max = 30;
        r_spread = 10;
    } else if (num_tasks <= 20) {
        p_min = 100; p_max = 700;
        q_min = 50;  q_max = 800;
        r_spread = 100;
    } else if (num_tasks <= 50) {
        p_min = 200; p_max = 1800;
        q_min = 50;  q_max = 1800;
        r_spread = 300;
    } else { // np. 100
        p_min = 200; p_max = 3700;
        q_min = 50;  q_max = 3700;
        r_spread = 500;
    }

    std::uniform_int_distribution<int> p_dist(p_min, p_max);
    std::uniform_int_distribution<int> q_dist(q_min, q_max);

    std::string folder_path = "test_data/average/n" + std::to_string(num_tasks);
    system(("mkdir -p " + folder_path).c_str());

    std::string filename = folder_path + "/data" + std::to_string(instance_num) + ".DAT";
    std::ofstream out(filename);

    out << num_tasks << "\n";

    int cumulative_p = 0;
    for (int i = 0; i < num_tasks; ++i) {
        int p = p_dist(gen);

        // Czas dostępności oparty o skumulowane p + losowość
        int r_mean = (i < 5) ? 0 : cumulative_p / i;
        std::uniform_int_distribution<int> r_dist(std::max(0, r_mean - r_spread), r_mean + r_spread);
        int r = r_dist(gen);

        int q = q_dist(gen);

        out << p << " " << r << " " << q << "\n";
        cumulative_p += p;
    }
}

int main() {
    std::vector<int> sizes = {6, 7, 8, 9, 10, 11, 12, 20, 50};
    int K = 50;

    for (int N : sizes) {
        std::cout << "Generowanie 50 instancji o rozmiarze N = " << N << "...\n";
        for (int i = 1; i <= K; ++i) {
            generateInstance(i, N);
        }
        std::cout << "✔️ Ukończono: N = " << N << "\n";
    }

    std::cout << "\n✅ Gotowe! Wszystkie instancje wygenerowane.\n";
    return 0;
}