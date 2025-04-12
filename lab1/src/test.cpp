#include "../include/test.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

int dataset_size[9] = {6, 7, 8, 9, 10, 11, 12, 20, 50};

void Test::runTest() {
    std::ofstream outFile("result.csv");
    outFile << "DataSize,SortR_Cmax,SortR_Time[ns],SortQ_Cmax,SortQ_Time[ns],Schrage_Cmax,Schrage_Time[ns],Schrage_Div_Cmax,Schrage_Div_Time[ns],BruteForce_Cmax,BruteForce_Time[ms],Custom_Cmax,Custom_Time[ns]\n";
    
    for(int i = 0; i < 9; i++) {
        std::string nazwaPliku = "../test_data/single/data" + std::to_string(i+1) + ".DAT";
        inst.WczytajZPliku(nazwaPliku);

        // Collect all Cmax values to find the best one
        std::vector<int> all_cmax;
        
        // Run all algorithms and store Cmax values
        auto sortR = inst.SortR();
        auto sortQ = inst.SortQ();
        auto schrage = inst.Schrage();
        auto schrageDiv = inst.SchrageZPodzialem();
        auto brute = (i <= 6) ? inst.Brute() : std::make_pair(-1, 0);
        auto custom = inst.Wlasny();

        // Store all Cmax values (excluding brute force for i > 5)
        all_cmax.push_back(sortR.first);
        all_cmax.push_back(sortQ.first);
        all_cmax.push_back(schrage.first);
        all_cmax.push_back(schrageDiv.first);
        if(i <= 6) all_cmax.push_back(brute.first);
        all_cmax.push_back(custom.first);

        // Find the best Cmax (minimum value)
        int best_cmax = *std::min_element(all_cmax.begin(), all_cmax.end());

        // Helper function to format output with error percentage
        auto formatWithError = [best_cmax](int cmax) {
            if(cmax == -1) return std::string("-");
            if(cmax == best_cmax) return std::to_string(cmax) + " [0%]";
            
            double error = 100.0 * (cmax - best_cmax) / best_cmax;
            return std::to_string(cmax) + " [" + std::to_string(static_cast<int>(std::round(error))) + "%]";
        };

        // Write results
        outFile << dataset_size[i] << ","
                << formatWithError(sortR.first) << "," << sortR.second << ","
                << formatWithError(sortQ.first) << "," << sortQ.second << ","
                << formatWithError(schrage.first) << "," << schrage.second << ","
                << schrageDiv.first << "," << schrageDiv.second << ",";
        
        if(i > 6) {
            outFile << "-,-";
        } else {
            outFile << formatWithError(brute.first) << "," << brute.second;
        }

        outFile << "," << formatWithError(custom.first) << "," << custom.second << "\n";
    }
    
    outFile.close();
}


void Test::runAverageTest() {
    std::ofstream outFile("average_results.csv");
    outFile << "DataSize,AvgSortR_Time[ns],AvgSortR_Error[%],AvgSortQ_Time[ns],AvgSortQ_Error[%],"
            << "AvgSchrage_Time[ns],AvgSchrage_Error[%],AvgSchrageDiv_Time[ns],AvgSchrageDiv_Error[%],"
            << "AvgBrute_Time[ms],AvgBrute_Error[%],AvgCustom_Time[ns],AvgCustom_Error[%]\n";

    for(int size_index = 0; size_index < 9; size_index++) {
        int current_size = dataset_size[size_index];
        int instance_count = 50;
        
        // Struktura do przechowywania sum
        struct Averages {
            long long sortR_time = 0, sortQ_time = 0, schrage_time = 0, schrageDiv_time = 0, brute_time = 0, custom_time = 0;
            double sortR_error = 0, sortQ_error = 0, schrage_error = 0, schrageDiv_error = 0, brute_error = 0, custom_error = 0;
            int valid_brute_instances = 0;
        } avg;

        for(int instance_num = 1; instance_num <= instance_count; instance_num++) {
            std::string nazwaPliku = "../test_data/average/n" + std::to_string(current_size) + 
                                    "/data" + std::to_string(instance_num) + ".DAT";
            inst.WczytajZPliku(nazwaPliku);

            // Uruchom wszystkie algorytmy
            auto sortR = inst.SortR();
            auto sortQ = inst.SortQ();
            auto schrage = inst.Schrage();
            auto schrageDiv = inst.SchrageZPodzialem();
            auto brute = (current_size <= 12) ? inst.Brute() : std::make_pair(-1, -1);
            auto custom = inst.Wlasny();

            // Znajdź najlepsze rozwiązanie
            std::vector<int> all_cmax = {sortR.first, sortQ.first, schrage.first, schrageDiv.first, custom.first};
            if(current_size <= 12) all_cmax.push_back(brute.first);
            int best_cmax = *std::min_element(all_cmax.begin(), all_cmax.end());

            // Funkcja do obliczania błędu
            auto calcError = [best_cmax](int cmax) {
                return (cmax == -1 || cmax == best_cmax) ? 0.0 : 
                       (100.0 * (cmax - best_cmax) / best_cmax);
            };

            // Akumuluj wyniki
            avg.sortR_time += sortR.second;
            avg.sortQ_time += sortQ.second;
            avg.schrage_time += schrage.second;
            avg.schrageDiv_time += schrageDiv.second;
            avg.custom_time += custom.second;
            
            if(current_size <= 12) {
                avg.brute_time += brute.second;
                avg.valid_brute_instances++;
            }

            avg.sortR_error += calcError(sortR.first);
            avg.sortQ_error += calcError(sortQ.first);
            avg.schrage_error += calcError(schrage.first);
            avg.schrageDiv_error += calcError(schrageDiv.first);
            avg.custom_error += calcError(custom.first);
            
            if(current_size <= 12) {
                avg.brute_error += calcError(brute.first);
            }
        }

        // Oblicz średnie i zapisz wyniki
        outFile << current_size << ","
                << avg.sortR_time / instance_count << ","
                << avg.sortR_error / instance_count << ","
                << avg.sortQ_time / instance_count << ","
                << avg.sortQ_error / instance_count << ","
                << avg.schrage_time / instance_count << ","
                << avg.schrage_error / instance_count << ","
                << avg.schrageDiv_time / instance_count << ","
                << "-" << ",";
        
        // Obsługa kolumny Brute Force
        if(current_size <= 12) {
            outFile << avg.brute_time / avg.valid_brute_instances << ","
                    << avg.brute_error / avg.valid_brute_instances << ",";
        } else {
            outFile << "-,-,";
        }

        outFile << avg.custom_time / instance_count << ","
                << avg.custom_error / instance_count << "\n";
    }
    
    outFile.close();
}