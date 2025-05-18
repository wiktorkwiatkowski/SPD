#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../inc/scheduler.h"

std::string Test::formatResult(int cmax, int bestCmax, long long time, bool isTimeMs) const {
    std::stringstream ss;

    if (cmax == -1) {
        ss << "-";
        return ss.str();
    }

    if (bestCmax == -1) {
        ss << cmax;
    } else {
        double error = 100.0 * (cmax - bestCmax) / bestCmax;
        ss << cmax << " [" << std::fixed << std::setprecision(0) << error << "%]";
    }

    ss << ",";

    if (isTimeMs) {
        ss << time / 1000.0;
    } else {
        ss << time;
    }

    return ss.str();
}

void Test::saveToCSV(const std::string& filename,
                     const std::vector<std::vector<std::string>>& data) {
    std::ofstream outFile(filename);

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i != 0) outFile << ",";
            outFile << row[i];
        }
        outFile << "\n";
    }

    outFile.close();
}

void Test::runTest() {
    std::vector<std::vector<std::string>> table1_results;
    std::vector<std::vector<std::string>> table2_results;

    // Nagłówki tabel
    table1_results.push_back({
        "Rozmiar instancji", "Przedział p_j",
        "LSA (Cmax i błąd)", "Czas LSA [μs]",
        "LPT (Cmax i błąd)", "Czas LPT [μs]",
        "PD (Cmax)", "Czas PD [μs]",
        "Przegląd zupełny (Cmax)", "Czas przeglądu [μs]"
    });

    table2_results.push_back({
        "Rozmiar instancji", "Przedział p_j",
        "PTAS k=n/2 (Cmax i błąd)", "Czas [μs]",
        "PTAS k=2n/3 (Cmax i błąd)", "Czas [μs]",
        "PTAS k=3n/4 (Cmax i błąd)", "Czas [μs]",
        "FPTAS ε=0.5 (Cmax i błąd)", "Czas [μs]",
        "FPTAS ε=0.33 (Cmax i błąd)", "Czas [μs]",
        "FPTAS ε=0.25 (Cmax i błąd)", "Czas [μs]"
    });

    std::vector<std::tuple<std::string, int, int, int>> testCases = {
        {"2/10", 10, 1, 10},  {"2/10", 10, 10, 20},  {"2/20", 20, 1, 10},
        {"2/20", 20, 10, 20}, {"2/20", 20, 50, 100}, {"2/50", 50, 1, 10},
        {"2/50", 50, 10, 20}, {"2/50", 50, 50, 100}
    };

    for (const auto& testCase : testCases) {
        std::string sizeDesc = std::get<0>(testCase);
        int tasksCount = std::get<1>(testCase);
        int minP = std::get<2>(testCase);
        int maxP = std::get<3>(testCase);

        scheduler.generateRandomInstance(2, tasksCount, minP, maxP);

        std::stringstream rangeDesc;
        rangeDesc << "[" << minP << "-" << maxP << "]";

        auto lsa = scheduler.calculateLSA();
        auto lpt = scheduler.calculateLPT();
        auto dp = scheduler.dynamicProgramming();
        auto brute = scheduler.bruteForce();

        int bestCmax = dp.first;
        if (brute.first != -1) {
            bestCmax = std::min(bestCmax, brute.first);
        }

        // Table 1
        std::vector<std::string> row1 = {sizeDesc, rangeDesc.str()};

        // LSA
        auto lsaStr = formatResult(lsa.first, bestCmax, lsa.second);
        auto pos = lsaStr.find(',');
        row1.push_back(lsaStr.substr(0, pos));
        row1.push_back(lsaStr.substr(pos + 1));

        // LPT
        auto lptStr = formatResult(lpt.first, bestCmax, lpt.second);
        pos = lptStr.find(',');
        row1.push_back(lptStr.substr(0, pos));
        row1.push_back(lptStr.substr(pos + 1));

        // DP (PD)
        auto dpStr = formatResult(dp.first, -1, dp.second);  // brak błędu
        pos = dpStr.find(',');
        row1.push_back(dpStr.substr(0, pos));
        row1.push_back(dpStr.substr(pos + 1));

        // Brute force
        auto bruteStr = formatResult(brute.first, -1, brute.second, true);
        pos = bruteStr.find(',');
        row1.push_back(bruteStr.substr(0, pos));
        row1.push_back(bruteStr.substr(pos + 1));

        table1_results.push_back(row1);

        // Table 2
        std::vector<std::string> row2 = {sizeDesc, rangeDesc.str()};

        // PTAS
        std::vector<int> ks = {tasksCount / 2, 2 * tasksCount / 3, 3 * tasksCount / 4};
        for (int k : ks) {
            auto ptas = scheduler.calculatePTAS(k);
            auto ptasStr = formatResult(ptas.first, bestCmax, ptas.second);
            pos = ptasStr.find(',');
            row2.push_back(ptasStr.substr(0, pos));
            row2.push_back(ptasStr.substr(pos + 1));
        }

        // FPTAS
        std::vector<double> epsilons = {0.5, 0.33, 0.25};
        for (double eps : epsilons) {
            auto fptas = scheduler.calculateFPTAS(eps);
            auto fptasStr = formatResult(fptas.first, bestCmax, fptas.second);
            pos = fptasStr.find(',');
            row2.push_back(fptasStr.substr(0, pos));
            row2.push_back(fptasStr.substr(pos + 1));
        }

        table2_results.push_back(row2);
    }

    saveToCSV("tabela1.csv", table1_results);
    saveToCSV("tabela2.csv", table2_results);

    std::cout << "Wyniki zapisane do plików tabela1.csv i tabela2.csv" << std::endl;
}
