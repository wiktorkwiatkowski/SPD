#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <climits>

#include "zadanie.hpp"

// Klasa Problem przechowuje zadania i implementuje różne algorytmy
class Problem {
    std::vector<Zadanie> zadania; // Wektor zadań
  
  public:
    // Funkcja wczytująca dane z pliku
    void WczytajZPliku(const std::string &nazwaPliku);
    void WyswietlInstancje();
    int PoliczCmax();
    int PoliczCmaxSortR();
    int PoliczCmaxSortQ();

    
    // Komparator dla kolejki N (sortowanie po najmniejszym r)
    struct CompareR {
      bool operator()(const Zadanie &a, const Zadanie &b) {
        return a.GetR() > b.GetR(); // Dla min-heap po r
      }
    };
  
    // Komparator dla kolejki G (sortowanie po największym q)
    struct CompareQ {
      bool operator()(const Zadanie &a, const Zadanie &b) {
        return a.GetQ() < b.GetQ(); // Dla max-heap po q
      }
    };
  
    // Metoda implementująca algorytm Schrage
    void Schrage();
  };