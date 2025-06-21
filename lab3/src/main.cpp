#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

#include "../inc/generator.h"
#include "../inc/instance_loader.h"
#include "../inc/scheduler.h"
#include "../inc/tester.h"

namespace fs = std::filesystem;

int main() {
    // JAK CHCESZ NOWE INSTANCJE TO ODKOMENTUJ

    /*
    Generator generator;
    std::vector<int> machine_counts = {2, 3};
    std::vector<int> job_counts = {5, 10, 15, 20};
    generator.generateMultipleInstances(machine_counts, job_counts);
    */

    std::vector<std::pair<std::string, FlowShopInstance>> all_instances;
    std::string folder_path = "../instances/";
    std::vector<fs::directory_entry> entries;
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".dat") {
            entries.push_back(entry);
        }
    }

    auto extractDims = [](const std::string& name) {
        size_t underscore = name.find('_');
        size_t x = name.find('x');
        int m = std::stoi(name.substr(underscore + 1, x - underscore - 1));
        int n = std::stoi(name.substr(x + 1));
        return std::make_pair(m, n);
    };

    std::sort(entries.begin(), entries.end(), [&](const auto& a, const auto& b) {
        auto a_name = a.path().stem().string();
        auto b_name = b.path().stem().string();
        return extractDims(a_name) < extractDims(b_name);  // sortuj po (maszyny, zadania)
    });

    // Teraz dopiero wczytuj
    for (const auto& entry : entries) {
        std::string path = entry.path().string();
        std::string name = entry.path().stem().string();
        auto instance = InstanceLoader::loadFromFile(path);
        all_instances.emplace_back(name, std::move(instance));
        std::cout << "Wczytano: " << name << "\n";
    }

    if (!all_instances.empty()) {
        const auto& [name, inst] = all_instances[0];

        auto [cmax, time] = Scheduler::bruteForce(inst);
        std::cout << "\n[Brute-force] Instancja: " << name << ", Cmax: " << cmax
                  << ", Czas: " << time << " μs\n";
    }

    Tester tester;
    tester.runBruteForceTest(all_instances);
}