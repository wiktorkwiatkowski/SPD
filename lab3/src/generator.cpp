#include "../inc/generator.h"
#include <fstream>
#include <iostream>
#include <filesystem>  // C++17

namespace fs = std::filesystem;

Generator::Generator(int minTime, int maxTime)
    : min_time(minTime), max_time(maxTime), rng(std::random_device{}()) {}

void Generator::generateInstance(int machines, int jobs, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Błąd: Nie można otworzyć pliku " << filename << "\n";
        return;
    }

    file << jobs << " " << machines << "\n";
    std::uniform_int_distribution<> dist(min_time, max_time);

    for (int j = 0; j < jobs; ++j) {
        for (int m = 0; m < machines; ++m) {
            int time = dist(rng);
            file << m << " " << time;
            if (m < machines - 1) file << " ";
        }
        file << "\n";
    }

    file.close();
    std::cout << "Wygenerowano instancję: " << filename << "\n";
}

void Generator::generateMultipleInstances(const std::vector<int>& machine_counts,
                                          const std::vector<int>& job_counts,
                                          const std::string& prefix) {
    // Upewnij się, że folder "instances" istnieje
    const std::string folder = "../instances";
    if (!fs::exists(folder)) {
        fs::create_directory(folder);
    }

    for (int m : machine_counts) {
        for (int j : job_counts) {
            std::string filename = folder + "/" + prefix + "_" + std::to_string(m) + "x" + std::to_string(j) + ".dat";
            generateInstance(m, j, filename);
        }
    }
}
