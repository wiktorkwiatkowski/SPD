#include "../inc/tester.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../inc/scheduler.h"

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

void Tester::runBruteForceAndNEH(
    const std::vector<std::pair<std::string, FlowShopInstance>>& instances) {
    std::vector<std::vector<std::string>> results;

    // Nagłówek
    results.push_back({"Nazwa instancji", "Liczba zadań", "Liczba maszyn", "NEH (Cmax)",
                       "Czas NEH [μs]", "FNEH (Cmax)", "Czas FNEH [μs]", "Johnson (Cmax)",
                       "Czas Johnson [μs]", "Brute Force (Cmax)", "Czas Brute Force [μs]"});

    for (const auto& [name, inst] : instances) {
        std::vector<std::string> row = {name, std::to_string(inst.num_jobs),
                                        std::to_string(inst.num_machines)};

        // --- Algorytmy ---
        auto neh = scheduler.calculateNEH(inst);
        auto fneh = scheduler.calculateFNEH(inst);

        std::pair<int, long long> brute = {-1, -1};
        if (inst.num_jobs <= 10) {
            brute = Scheduler::bruteForce(inst);
        }

        std::pair<int, long long> johnson = {-1, -1};
        if (inst.num_machines == 2) {
            johnson = scheduler.johnson(inst);
        }

        // Użyj Cmax z brute-force jako wzorcowego dla obliczania błędów
        int bestCmax = (brute.first != -1) ? brute.first : -1;

        // --- NEH ---
        auto nehStr = formatResult(neh.first, bestCmax, neh.second);
        std::cout << "Plik: " << name << ", brute: " << brute.first << ", neh: " << neh.first
                  << ", fneh: " << fneh.first << ", johonson: "<<johnson.first<< std::endl;
        auto pos = nehStr.find(',');
        row.push_back(nehStr.substr(0, pos));
        row.push_back(nehStr.substr(pos + 1));

        // --- FNEH ---
        auto fnehStr = formatResult(fneh.first, bestCmax, fneh.second);
        pos = fnehStr.find(',');
        row.push_back(fnehStr.substr(0, pos));
        row.push_back(fnehStr.substr(pos + 1));

        // --- Johnson ---
        if (johnson.first != -1) {
            auto johnsonStr = formatResult(johnson.first, bestCmax, johnson.second);
            pos = johnsonStr.find(',');
            row.push_back(johnsonStr.substr(0, pos));
            row.push_back(johnsonStr.substr(pos + 1));
        } else {
            row.push_back("-");
            row.push_back("-");
        }

        // --- Brute Force ---
        if (brute.first != -1) {
            auto bruteStr = formatResult(brute.first, -1, brute.second);
            pos = bruteStr.find(',');
            row.push_back(bruteStr.substr(0, pos));
            row.push_back(bruteStr.substr(pos + 1));
        } else {
            row.push_back("-");
            row.push_back("-");
        }

        results.push_back(row);
    }

    saveToCSV("tabela_brute_neh_fneh_johnson.csv", results);
    std::cout << "✓ Wyniki zapisane do tabela_brute_neh_fneh_johnson.csv\n";
}
