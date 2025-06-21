#include "../inc/tester.h"
#include "../inc/scheduler.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

void Tester::runBruteForceTest(const std::vector<std::pair<std::string, FlowShopInstance>>& instances) {
    std::vector<std::vector<std::string>> results;

    // Nagłówek
    results.push_back({
        "Nazwa instancji", "Liczba zadań", "Liczba maszyn",
        "Brute-force (Cmax)", "Czas [μs]"
    });

    for (const auto& [name, inst] : instances) {
        auto [cmax, time] = Scheduler::bruteForce(inst);

        std::string formatted = formatResult(cmax, -1, time);  // bez błędu
        auto pos = formatted.find(',');
        std::string cmaxStr = formatted.substr(0, pos);
        std::string timeStr = formatted.substr(pos + 1);

        results.push_back({
            name,
            std::to_string(inst.num_jobs),
            std::to_string(inst.num_machines),
            cmaxStr,
            timeStr
        });

        std::cout << "✓ " << name << " -> Cmax: " << cmaxStr
                  << ", czas: " << timeStr << " μs\n";
    }

    saveToCSV("results.csv", results);
    std::cout << "\nWyniki zapisane do tabela_brute.csv\n";
}

std::string Tester::formatResult(int cmax, int bestCmax, long long time, bool isTimeMs) const {
    std::stringstream ss;

    if (cmax == -1) return "-";

    if (bestCmax == -1) {
        ss << cmax;
    } else {
        double error = 100.0 * (cmax - bestCmax) / bestCmax;
        ss << cmax << " [" << std::fixed << std::setprecision(0) << error << "%]";
    }

    ss << ",";
    if (isTimeMs)
        ss << time / 1000.0;
    else
        ss << time;

    return ss.str();
}

void Tester::saveToCSV(const std::string& filename,
                       const std::vector<std::vector<std::string>>& data) const {
    std::ofstream outFile(filename);
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) outFile << ",";
            outFile << row[i];
        }
        outFile << "\n";
    }
}
