#include "../include/test.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

#include <iomanip>  // dla std::setprecision
#include <sstream>  // dla std::ostringstream

int dataset_size[9] = {6, 7, 8, 9, 10, 11, 12, 20, 50};

void Test::runTest() {
    std::ofstream outFile("result_vertical.csv");

    // Nagłówek: nazwa algorytmu + kolumny z rozmiarami danych
    std::vector<std::string> headers = { "SortR Cmax[%]", "SortR Time[ns]",
                                         "SortQ Cmax[%]", "SortQ Time[ns]",
                                         "Schrage Cmax[%]", "Schrage Time[ns]",
                                         "SchrageDiv Cmax[%]", "SchrageDiv Time[ns]",
                                         "BruteForce Cmax[%]", "BruteForce Time[ms]",
                                         "Custom Cmax[%]", "Custom Time[ns]" };

    std::vector<std::vector<std::string>> results(12, std::vector<std::string>(9));

    for (int i = 0; i < 9; i++) {
        std::string filename = "../test_data/single/data" + std::to_string(i + 1) + ".DAT";
        inst.WczytajZPliku(filename);

        auto sortR = inst.SortR();
        auto sortQ = inst.SortQ();
        auto schrage = inst.Schrage();
        auto schrageDiv = inst.SchrageZPodzialem();
        auto brute = (i <= 6) ? inst.Brute() : std::make_pair(-1, -1);
        auto custom = inst.Wlasny();

        // Wyznacz najlepszy cmax z pominięciem Schrage z podziałem
        std::vector<int> cmax_for_best = { sortR.first, sortQ.first, schrage.first, custom.first };
        if (i <= 6 && brute.first != -1) cmax_for_best.push_back(brute.first);

        int best_cmax = *std::min_element(cmax_for_best.begin(), cmax_for_best.end());

        auto formatWithError = [best_cmax](int cmax) -> std::string {
            if (cmax == -1) return "-";
            if (cmax == best_cmax) return std::to_string(cmax) + " [0%]";
            double error = 100.0 * (cmax - best_cmax) / best_cmax;
            return std::to_string(cmax) + " [" + std::to_string(static_cast<int>(std::round(error))) + "%]";
        };

        // Wypełnij wyniki
        results[0][i]  = formatWithError(sortR.first);
        results[1][i]  = std::to_string(sortR.second);

        results[2][i]  = formatWithError(sortQ.first);
        results[3][i]  = std::to_string(sortQ.second);

        results[4][i]  = formatWithError(schrage.first);
        results[5][i]  = std::to_string(schrage.second);

        results[6][i]  = schrageDiv.first == -1 ? "-" : std::to_string(schrageDiv.first) + " [-]";
        results[7][i]  = std::to_string(schrageDiv.second);

        results[8][i]  = (i > 6 || brute.first == -1) ? "-" : formatWithError(brute.first);
        results[9][i]  = (i > 6 || brute.first == -1) ? "-" : std::to_string(brute.second);

        results[10][i] = formatWithError(custom.first);
        results[11][i] = std::to_string(custom.second);
    }

    // Wypisz nagłówki
    outFile << "Algorithm/DataSize";
    for (int i = 0; i < 9; ++i)
        outFile << "," << dataset_size[i];
    outFile << "\n";

    // Wypisz dane
    for (int row = 0; row < 12; ++row) {
        outFile << headers[row];
        for (int col = 0; col < 9; ++col) {
            outFile << "," << results[row][col];
        }
        outFile << "\n";
    }

    outFile.close();
}


void Test::runAverageTest() {
    std::ofstream avgOut("average_results_vertical.csv");
    std::ofstream maxOut("max_error_results_vertical.csv");

    avgOut << "Algorithm,6,7,8,9,10,11,12,20,50\n";
    maxOut << "Algorithm,6,7,8,9,10,11,12,20,50\n";

    std::vector<std::vector<std::string>> avg_data(12, std::vector<std::string>(9, "-"));
    std::vector<std::vector<std::string>> max_data(12, std::vector<std::string>(9, "-"));

    for(int size_index = 0; size_index < 9; size_index++) {
        int current_size = dataset_size[size_index];
        int instance_count = 50;
        int loaded_instances = 0;

        struct Aggregates {
            long long time_sum = 0;
            double error_sum = 0;
            double max_error = 0;
        };

        std::vector<Aggregates> aggr(6); // 0: SortR, 1: SortQ, 2: Schrage, 3: SchrageDiv, 4: Brute, 5: Custom

        for(int inst_num = 1; inst_num <= instance_count; inst_num++) {
            std::string filename = "../test_data/average/n" + std::to_string(current_size) + "/data" + std::to_string(inst_num) + ".DAT";

            inst.WczytajZPliku(filename);
            loaded_instances++;

            auto sortR = inst.SortR();
            auto sortQ = inst.SortQ();
            auto schrage = inst.Schrage();
            auto schrageDiv = inst.SchrageZPodzialem();
            auto brute = (current_size <= 12) ? inst.Brute() : std::make_pair(-1, -1);
            auto custom = inst.Wlasny();

            // Zbieraj tylko cmaxy z algorytmów NIE-preemptive
            std::vector<int> cmax_candidates;
            cmax_candidates.push_back(sortR.first);
            cmax_candidates.push_back(sortQ.first);
            cmax_candidates.push_back(schrage.first);
            cmax_candidates.push_back(custom.first);
            if(current_size <= 12 && brute.first != -1) {
                cmax_candidates.push_back(brute.first);
            }

            int best_cmax = *std::min_element(cmax_candidates.begin(), cmax_candidates.end());

            auto calcError = [best_cmax](int cmax) -> double {
                if(cmax == -1 || best_cmax == 0) return 0.0;
                return 100.0 * (cmax - best_cmax) / best_cmax;
            };

            // Akumulacja wyników
            std::pair<int, long long> algs[6] = {sortR, sortQ, schrage, schrageDiv, brute, custom};
            for (int j = 0; j < 6; ++j) {
                if (algs[j].first == -1) continue;
                aggr[j].time_sum += algs[j].second;

                // dla Schrage z podziałem – nie licz błędu
                if (j == 3) continue;

                double err = calcError(algs[j].first);
                aggr[j].error_sum += err;
                aggr[j].max_error = std::max(aggr[j].max_error, err);
            }
        }

        if (loaded_instances == 0) continue;

        auto format = [](double val) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << val;
            return oss.str();
        };

        // Wypełnianie danych do tabeli
        for (int i = 0; i < 6; ++i) {
            int avg_i = i * 2;
            if (i == 4 && current_size > 12) continue;  // brute dla n > 12 pomijamy

            avg_data[avg_i][size_index] = format(aggr[i].time_sum / (double)loaded_instances);

            if (i == 3) {
                // Schrage z podziałem — błąd oznaczany jako "-"
                avg_data[avg_i + 1][size_index] = "-";
                max_data[avg_i + 1][size_index] = "-";
            } else {
                avg_data[avg_i + 1][size_index] = format(aggr[i].error_sum / loaded_instances);
                max_data[avg_i + 1][size_index] = format(aggr[i].max_error);
            }
        }
    }

    // Nagłówki
    std::vector<std::string> headers = {
        "AvgSortR Time[ns]", "AvgSortR Error[%]",
        "AvgSortQ Time[ns]", "AvgSortQ Error[%]",
        "AvgSchrage Time[ns]", "AvgSchrage Error[%]",
        "AvgSchrageDiv Time[ns]", "AvgSchrageDiv Error[%]",
        "AvgBrute Time[ms]", "AvgBrute Error[%]",
        "AvgCustom Time[ns]", "AvgCustom Error[%]"
    };

    // Zapisz do pliku średnie
    for (int i = 0; i < 12; ++i) {
        avgOut << headers[i];
        for (int j = 0; j < 9; ++j) {
            avgOut << "," << avg_data[i][j];
        }
        avgOut << "\n";
    }

    // Zapisz do pliku maksymalne błędy
    for (int i = 1; i < 12; i += 2) {  // tylko błędy
        maxOut << headers[i];
        for (int j = 0; j < 9; ++j) {
            maxOut << "," << max_data[i][j];
        }
        maxOut << "\n";
    }

    avgOut.close();
    maxOut.close();
}