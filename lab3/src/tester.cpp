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
    
    std::vector<std::vector<std::string>> results_2machines;
    std::vector<std::vector<std::string>> results_3machines;

    // Nagłówki (zmieniona kolejność kolumn)
    std::vector<std::string> header_2 = {
        "Liczba zadań",       "Liczba maszyn",
        "Brute Force (Cmax)", "Czas Brute Force [μs]",
        "NEH (Cmax)",         "Czas NEH [μs]",
        "Johnson (Cmax)",     "Czas Johnson [μs]",
        "FNEH (Cmax)",        "Czas FNEH [μs]"
    };

    std::vector<std::string> header_3 = {
        "Liczba zadań",       "Liczba maszyn",
        "Brute Force (Cmax)", "Czas Brute Force [μs]",
        "NEH (Cmax)",         "Czas NEH [μs]",
        "FNEH (Cmax)",        "Czas FNEH [μs]"
    };

    results_2machines.push_back(header_2);
    results_3machines.push_back(header_3);

    for (const auto& [name, inst] : instances) {
        std::vector<std::string> row = {
            std::to_string(inst.num_jobs),
            std::to_string(inst.num_machines)
        };

        // Algorytmy
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

        // Dobór najlepszego Cmax do porównania
        int bestCmax = (brute.first != -1) ? brute.first : neh.first;

        // --- Brute Force ---
        if (brute.first != -1) {
            row.push_back(std::to_string(brute.first));
            row.push_back(std::to_string(brute.second));
        } else {
            row.push_back("-");
            row.push_back("-");
        }

        // --- NEH ---
        if (inst.num_jobs > 10)
            row.push_back(formatResult(neh.first, -1, neh.second, false));  // bez %
        else
            row.push_back(formatResult(neh.first, bestCmax, neh.second, false));
        
        // --- Johnson (tylko dla 2 maszyn) ---
        if (inst.num_machines == 2) {
            if (johnson.first != -1) {
                row.push_back(formatResult(johnson.first, bestCmax, johnson.second, false));
            } else {
                row.push_back("-");
                row.push_back("-");
            }
        }

        // --- FNEH ---
        row.push_back(formatResult(fneh.first, bestCmax, fneh.second, false));

        // --- Przypisz do odpowiedniej tabeli ---
        if (inst.num_machines == 2) {
            results_2machines.push_back(row);
        } else {
            results_3machines.push_back(row);
        }
    }

    saveToCSV("tabela_2maszyny.csv", results_2machines);
    saveToCSV("tabela_3maszyny.csv", results_3machines);
    std::cout << "✓ Zapisano tabele do tabela_2maszyny.csv i tabela_3maszyny.csv\n";
}

