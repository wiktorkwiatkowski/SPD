#include "../include/test.hpp"

#include <fstream>


// to co printuje jest w formacie <int, int> - Cmax i czas
// po kolei przeprowadzane sa testy kazdego algorytmu
// zapisywane sa w formacie mniej wiecej jak tabelka ze wzoru sprawozdania
// trzeba bedzie dodac jeszcze tylko te procenty
// wyniki zapisywane są do pliku result.csv który jest w folderze build


void Test::runTest() {
    // Open the output file
    std::ofstream outFile("result.csv");
    
    // Write CSV header
    outFile << "TestNo,SortR_Cmax,SortR_Time,SortQ_Cmax,SortQ_Time,Schrage_Cmax,Schrage_Time,BruteForce_Cmax,BruteForce_Time\n";
    
    for(int i = 0; i < 8; i++) {
        std::string nazwaPliku = "../test_data/TEST";
        nazwaPliku += std::to_string(i+1);
        nazwaPliku += ".DAT";
        inst.WczytajZPliku(nazwaPliku);

        // Write test number
        outFile << i+1 << ",";

        // Sort R
        std::pair<int,int> wyniki = inst.PoliczCmaxSortR();
        outFile << wyniki.first << "," << wyniki.second << ",";

        // Sort Q
        wyniki = inst.PoliczCmaxSortQ();
        outFile << wyniki.first << "," << wyniki.second << ",";

        // Schrage
        wyniki = inst.Schrage();
        outFile << wyniki.first << "," << wyniki.second << ",";

        // Brute Force (only for first 6 tests)
        if(i > 5) {
            outFile << "-,-";
        } else {
            wyniki = inst.PoliczCmax();
            outFile << wyniki.first << "," << wyniki.second;
        }

        outFile << "\n";
    }
    
    // Close the file
    outFile.close();
}