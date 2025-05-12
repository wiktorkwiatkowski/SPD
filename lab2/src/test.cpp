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

    outFile << "Rozmiar instancji (liczba maszyn i zadań),przedział p_j,"
            << "Wartość kryterium LSA,Czas dział. alg. LSA [μs],"
            << "Wartość kryterium LPT,Czas dział. alg. LPT [μs],"
            << "Wartość kryterium PD,Czas dział. alg. PD [μs],"
            << "P2||Cmax,Czas dział. alg. Przegląd zupełny [ms],"
            << "Wartość PTAS,Czas dział. PTAS [μs]," 
            << "Wartość FPTAS,Czas dział. FPTAS [μs]\n";

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
    std::vector<std::vector<std::string>> results;

    std::vector<std::tuple<std::string, int, int, int>> testCases = {
        {"2/10", 10, 1, 10},  {"2/10", 10, 10, 20},  {"2/20", 20, 1, 10},
        {"2/20", 20, 10, 20}, {"2/20", 20, 50, 100}, {"2/30", 27, 50, 100},
        {"2/50", 50, 1, 10},  {"2/50", 50, 10, 20},  {"2/50", 50, 50, 100}};

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

        // PTAS (k = 8), FPTAS (ε = 0.2)
        auto ptas = scheduler.calculatePTAS(8);
        auto fptas = scheduler.calculateFPTAS(0.2);

        std::vector<std::string> row;
        row.push_back(sizeDesc);
        row.push_back(rangeDesc.str());

        // LSA results
        std::string lsaStr = formatResult(lsa.first, bestCmax, lsa.second);
        size_t pos = lsaStr.find(',');
        row.push_back(lsaStr.substr(0, pos));
        row.push_back(lsaStr.substr(pos + 1));

        // LPT results
        std::string lptStr = formatResult(lpt.first, bestCmax, lpt.second);
        pos = lptStr.find(',');
        row.push_back(lptStr.substr(0, pos));
        row.push_back(lptStr.substr(pos + 1));

        // DP results
        std::string dpStr = formatResult(dp.first, bestCmax, dp.second);
        pos = dpStr.find(',');
        row.push_back(dpStr.substr(0, pos));
        row.push_back(dpStr.substr(pos + 1));

        // Brute force results
        std::string bruteStr = formatResult(brute.first, bestCmax, brute.second, true);
        pos = bruteStr.find(',');
        row.push_back(bruteStr.substr(0, pos));
        row.push_back(bruteStr.substr(pos + 1));

        // PTAS
        auto ptasStr = formatResult(ptas.first, bestCmax, ptas.second);
        pos = ptasStr.find(',');
        row.push_back(ptasStr.substr(0, pos));
        row.push_back(ptasStr.substr(pos + 1));

        // FPTAS
        auto fptasStr = formatResult(fptas.first, bestCmax, fptas.second);
        pos = fptasStr.find(',');
        row.push_back(fptasStr.substr(0, pos));
        row.push_back(fptasStr.substr(pos + 1));

        results.push_back(row);
    }

    saveToCSV("pm_results.csv", results);
    std::cout << "Wyniki zapisane do pliku pm_results.csv" << std::endl;
}