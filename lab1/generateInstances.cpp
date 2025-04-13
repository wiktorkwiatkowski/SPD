#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <iomanip>

// Struktura reprezentująca pojedyncze zadanie
// Składa się z trzech parametrów:
// p - czas przetwarzania (processing time)
// r - czas dostępności (ready time)
// q - czas dostarczenia (delivery time)
struct Task {
    int p, r, q;
};

// Funkcja generująca pojedynczą instancję problemu
// instance_num - numer instancji (używany w nazwie pliku)
// num_tasks - liczba zadań do wygenerowania
void generateInstance(int instance_num, int num_tasks) {
    // Inicjalizacja generatora liczb pseudolosowych
    std::random_device rd;  // Źródło entropii
    std::mt19937 gen(rd()); // Silnik Mersenne Twister

    // Dostosowanie zakresów wartości parametrów w zależności od rozmiaru problemu
    int p_min, p_max, q_min, q_max, r_spread;
    if (num_tasks <= 10) {
        p_min = 1; p_max = 30;
        q_min = 1; q_max = 30;
        r_spread = 10;  // Zakres losowania czasu dostępności
    } else if (num_tasks <= 20) {
        p_min = 100; p_max = 700;
        q_min = 50;  q_max = 800;
        r_spread = 100;
    } else if (num_tasks <= 50) {
        p_min = 200; p_max = 1800;
        q_min = 50;  q_max = 1800;
        r_spread = 300;
    } else { // Dla większych instancji (np. 100 zadań)
        p_min = 200; p_max = 3700;
        q_min = 50;  q_max = 3700;
        r_spread = 500;
    }

    // Inicjalizacja rozkładów prawdopodobieństwa
    std::uniform_int_distribution<int> p_dist(p_min, p_max); // Dla czasu przetwarzania
    std::uniform_int_distribution<int> q_dist(q_min, q_max); // Dla czasu dostarczenia

    // Przygotowanie ścieżki i nazwy pliku wynikowego
    std::string folder_path = "test_data/average/n" + std::to_string(num_tasks);
    system(("mkdir -p " + folder_path).c_str()); // Utworzenie folderu jeśli nie istnieje

    std::string filename = folder_path + "/data" + std::to_string(instance_num) + ".DAT";
    std::ofstream out(filename); // Otwarcie strumienia do zapisu

    // Nagłówek pliku - liczba zadań
    out << num_tasks << "\n";

    int cumulative_p = 0; // Suma czasów przetwarzania wszystkich dotychczasowych zadań
    for (int i = 0; i < num_tasks; ++i) {
        // Generowanie czasu przetwarzania
        int p = p_dist(gen);

        // Generowanie czasu dostępności:
        // - Dla pierwszych 5 zadań r = 0 (dostępne od początku)
        // - Dla pozostałych: średnia z dotychczasowych p + losowe odchylenie
        int r_mean = (i < 5) ? 0 : cumulative_p / i;
        std::uniform_int_distribution<int> r_dist(std::max(0, r_mean - r_spread), r_mean + r_spread);
        int r = r_dist(gen);

        // Generowanie czasu dostarczenia
        int q = q_dist(gen);

        // Zapis zadania do pliku
        out << p << " " << r << " " << q << "\n";
        cumulative_p += p; // Aktualizacja skumulowanego czasu przetwarzania
    }
}

int main() {
    // Wektor rozmiarów instancji do wygenerowania
    std::vector<int> sizes = {6, 7, 8, 9, 10, 11, 12, 20, 50};
    int K = 50; // Liczba instancji każdego rozmiaru

    // Generowanie instancji dla każdego rozmiaru
    for (int N : sizes) {
        std::cout << "Generowanie 50 instancji o rozmiarze N = " << N << "...\n";
        for (int i = 1; i <= K; ++i) {
            generateInstance(i, N);
        }
        std::cout << "Ukończono: N = " << N << "\n";
    }

    std::cout << "\nGotowe! Wszystkie instancje wygenerowane.\n";
    return 0;
}