#include "../include/test.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

#include <iomanip>  // dla std::setprecision
#include <sstream>  // dla std::ostringstream

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
    std::ofstream outFile("average_results_vertical.csv");
    outFile << "Algorithm,6,7,8,9,10,11,12,20,50\n";
    
    std::vector<std::vector<std::string>> data(12, std::vector<std::string>(9, "-"));

    for(int size_index = 0; size_index < 9; size_index++) {
        int current_size = dataset_size[size_index];
        int instance_count = 50;
        int loaded_instances = 0;
        
        struct Averages {
            long long sortR_time = 0, sortQ_time = 0, schrage_time = 0, 
                     schrageDiv_time = 0, brute_time = 0, custom_time = 0;
            double sortR_error = 0, sortQ_error = 0, schrage_error = 0, 
                   schrageDiv_error = 0, brute_error = 0, custom_error = 0;
        } avg;

        for(int instance_num = 1; instance_num <= instance_count; instance_num++) {
            std::string nazwaPliku = "../test_data/average/n" + std::to_string(current_size) + 
                                    "/data" + std::to_string(instance_num) + ".DAT";
            
            inst.WczytajZPliku(nazwaPliku);
            loaded_instances++;

            auto sortR = inst.SortR();
            auto sortQ = inst.SortQ();
            auto schrage = inst.Schrage();
            auto schrageDiv = inst.SchrageZPodzialem();
            auto brute = (current_size <= 12) ? inst.Brute() : std::make_pair(-1, -1);
            auto custom = inst.Wlasny();

            std::vector<int> all_cmax = {sortR.first, sortQ.first, schrage.first, schrageDiv.first, custom.first};
            if(current_size <= 12 && brute.first != -1) {
                all_cmax.push_back(brute.first);
            }
            
            if(all_cmax.empty()) continue;

            int best_cmax = *std::min_element(all_cmax.begin(), all_cmax.end());

            auto calcError = [best_cmax](int cmax) {
                if(cmax == -1) return 0.0;
                return 100.0 * (cmax - best_cmax) / best_cmax;
            };

            // Akumuluj wyniki
            avg.sortR_time += sortR.second;
            avg.sortQ_time += sortQ.second;
            avg.schrage_time += schrage.second;
            avg.schrageDiv_time += schrageDiv.second;
            avg.custom_time += custom.second;
            
            if(current_size <= 12 && brute.first != -1) {
                avg.brute_time += brute.second;
            }

            avg.sortR_error += calcError(sortR.first);
            avg.sortQ_error += calcError(sortQ.first);
            avg.schrage_error += calcError(schrage.first);
            avg.schrageDiv_error += calcError(schrageDiv.first);
            avg.custom_error += calcError(custom.first);
            
            if(current_size <= 12 && brute.first != -1) {
                avg.brute_error += calcError(brute.first);
            }
        }

        if(loaded_instances == 0) continue;

        // Funkcja do formatowania z 2 miejscami po przecinku
        auto formatTwoDecimals = [](double value) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2);
            oss << value;
            return oss.str();
        };

        // Oblicz i formatuj średnie
        data[0][size_index] = formatTwoDecimals(avg.sortR_time / (double)loaded_instances);
        data[1][size_index] = formatTwoDecimals(avg.sortR_error / loaded_instances);
        data[2][size_index] = formatTwoDecimals(avg.sortQ_time / (double)loaded_instances);
        data[3][size_index] = formatTwoDecimals(avg.sortQ_error / loaded_instances);
        data[4][size_index] = formatTwoDecimals(avg.schrage_time / (double)loaded_instances);
        data[5][size_index] = formatTwoDecimals(avg.schrage_error / loaded_instances);
        data[6][size_index] = formatTwoDecimals(avg.schrageDiv_time / (double)loaded_instances);
        data[7][size_index] = formatTwoDecimals(avg.schrageDiv_error / loaded_instances);
        
        if(current_size <= 12) {
            int valid_brute = (current_size <= 12) ? loaded_instances : 0;
            if(valid_brute > 0) {
                data[8][size_index] = formatTwoDecimals(avg.brute_time / (double)valid_brute);
                data[9][size_index] = formatTwoDecimals(avg.brute_error / valid_brute);
            }
        }
        
        data[10][size_index] = formatTwoDecimals(avg.custom_time / (double)loaded_instances);
        data[11][size_index] = formatTwoDecimals(avg.custom_error / loaded_instances);
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

    // Zapisz dane
    for(int i = 0; i < 12; i++) {
        outFile << headers[i];
        for(int j = 0; j < 9; j++) {
            outFile << "," << data[i][j];
        }
        outFile << "\n";
    }
    
    outFile.close();
}