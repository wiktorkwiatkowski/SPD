#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <climits>
#include <chrono>

#include "zadanie.hpp"

// Klasa Problem przechowuje zadania i implementuje różne algorytmy
class Problem {
    std::vector<Zadanie> zadania; // Wektor zadań
  
  public:
    // Funkcja wczytująca dane z pliku
    void WczytajZPliku(const std::string &nazwaPliku);
    // Funkcja wyświetlająca dane zadań
    void WyswietlInstancje();
    // Funkcja licząca Cmax dla wszystkich możliwych permutacji (przegląd zupełny - brute force)
    std::pair<int,int> Brute();
    // Funkcja licząca Cmax dla zadań posortowanych według r
    std::pair<int,int>  SortR();
    // Funkcja licząca Cmax dla zadań posortowanych według q
    std::pair<int,int> SortQ();
    // Funkcja licząca Cmax dla zadań posortowanych według wlasnego algorytmu
    std::pair<int,int> Wlasny();

    
    // Komparator dla kolejki N (sortowanie po najmniejszym r)
    struct CompareR {
      bool operator()(const Zadanie &a, const Zadanie &b) {
        return a.GetR() > b.GetR();
      }
    };
  
    // Komparator dla kolejki G (sortowanie po największym q)
    struct CompareQ {
      bool operator()(const Zadanie &a, const Zadanie &b) {
        return a.GetQ() < b.GetQ();
      }
    };
  
    // Metoda implementująca algorytm Schrage
    std::pair<int,int> Schrage();
    // Metoda implementująca algorytm Schrage z podziałem
    std::pair<int,int> SchrageZPodzialem();
  };