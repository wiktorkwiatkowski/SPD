#include "../include/problem.hpp"

// Klasa Zadanie przechowuje informacje o zadaniu (j - numer, r - dostępność, p
// - czas trwania, q - stygnięcie)
void Problem::WczytajZPliku(const std::string &nazwaPliku) {
    std::ifstream plik(nazwaPliku);
    if (!plik) {
      std::cerr << "Nie mozna otworzyc pliku!" << std::endl;
      return;
    }

    int n;
    plik >> n; // Wczytaj liczbę zadań
    zadania.clear();

    for (int i = 0; i < n; i++) {
      int r, p, q;
      plik >> r >> p >> q;
      zadania.emplace_back(i + 1, r, p, q);
    }
    plik.close();
}

    // Funkcja wyświetlająca dane zadań
void Problem::WyswietlInstancje() {
    for (const auto &zadanie : zadania) {
      std::cout << "j: " << zadanie.GetJ() << " | r: " << zadanie.GetR()
                << " p: " << zadanie.GetP() << " q: " << zadanie.GetQ()
                << std::endl;
    }
    std::cout << "============================================" << std::endl;
}
    // Funkcja licząca Cmax dla wszystkich możliwych permutacji (przegląd zupełny)
int Problem::PoliczCmax() {
    int minC = INT_MAX; // Inicjalizacja minimalnego Cmax
    std::vector<Zadanie> bestOrder;
    std::vector<Zadanie> perm = zadania;

    // Sortowanie początkowe w kolejności leksykograficznej (po j)
    std::sort(perm.begin(), perm.end());

    do {
      int C = 0;
      int actual_time = 0;

      // Obliczanie wartości Cmax dla danej permutacji
      for (const auto &zadanie : perm) {
        actual_time = std::max(actual_time, zadanie.GetR()) + zadanie.GetP();
        C = std::max(C, actual_time + zadanie.GetQ());
      }

      // Aktualizacja minimalnej wartości Cmax
      if (C < minC) {
        minC = C;
        bestOrder = perm;
      }

    } while (std::next_permutation(
        perm.begin(), perm.end())); // Generowanie kolejnych permutacji

    // Wyświetlanie najlepszego rozwiązania
    std::cout << "Minimum Cmax dla wszystkich permutacji: " << minC
              << std::endl;
    std::cout << "Rozwiazanie: ";
    for (const auto &zadanie : bestOrder) {
      std::cout << zadanie.GetJ() << " ";
    }
    std::cout << std::endl;

    return minC;
}
    // Obliczanie Cmax po posortowaniu zadań według dostępności `r`
int Problem::PoliczCmaxSortR() {
    std::vector<Zadanie> perm = zadania;
    std::sort(perm.begin(), perm.end(), [](const Zadanie &a, const Zadanie &b) {
      return a.GetR() < b.GetR();
    });

    int C = 0, actual_time = 0;
    for (const auto &zadanie : perm) {
      actual_time = std::max(actual_time, zadanie.GetR()) + zadanie.GetP();
      C = std::max(C, actual_time + zadanie.GetQ());
    }

    std::cout << "Cmax dla sortowania po r: " << C << std::endl;
    return C;
}


// Obliczanie Cmax po posortowaniu zadań według czasu stygnięcia `q`
int Problem::PoliczCmaxSortQ() {
    std::vector<Zadanie> perm = zadania;
    std::sort(perm.begin(), perm.end(), [](const Zadanie &a, const Zadanie &b) {
      return a.GetQ() > b.GetQ();
    });

    int C = 0, actual_time = 0;
    for (const auto &zadanie : perm) {
      actual_time = std::max(actual_time, zadanie.GetR()) + zadanie.GetP();
      C = std::max(C, actual_time + zadanie.GetQ());
    }

    std::cout << "Cmax dla sortowania po q: " << C << std::endl;
    return C;
}

void Problem::Schrage() {
    // Lambda do sortowania N (min-heap po r)
    auto compareR = [](const Zadanie &a, const Zadanie &b) {
      return a.GetR() > b.GetR();
    };

    // Lambda do sortowania G (max-heap po q)
    auto compareQ = [](const Zadanie &a, const Zadanie &b) {
      return a.GetQ() < b.GetQ();
    };

    // Kolejka priorytetowa `N` przechowuje zadania nieuszeregowane, sortowane
    // według wartości `r` (czas dostępności zadania).
    // Jest to tzw. min-heap, czyli element o najmniejszym `r` znajduje się na
    // szczycie kolejki.
    //
    // `priority_queue` domyślnie działa jako max-heap (największy element na
    // szczycie). Odwracamy porządek sortowania(przez lambde compareR), aby
    // uzyskać min-heap: najmniejsze `r` na szczycie. Odrazu iniciujemy kolejke
    // zadaniami i sortujemy lamdą

    std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareR)> N(compareR, zadania);

    // Kolejka priorytetowa `G` przechowuje zadania gotowe do wykonania,
    // sortowane według `q` (czas dostarczenia zadania).
    // Jest to max-heap, czyli element o największym `q` znajduje się na
    // szczycie kolejki.
    //
    // Kolejka `G` jest początkowo pusta, ponieważ nie przekazujemy do niej
    // żadnych danych przy inicjalizacji.
    // Zadania będą dodawane dynamicznie podczas działania algorytmu Schrage.
    //
    std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareQ)> G(compareQ);

    int t = 0;    // Aktualny czas
    int Cmax = 0; // Maksymalny czas dostarczenia

    while (!G.empty() || !N.empty()) {
      // Przenieś zadania z N do G jeśli r <= t (kroki 3-4 algorytmu)
      while (!N.empty() && N.top().GetR() <= t) {
        G.push(N.top());
        N.pop();
      }

      if (G.empty()) {
        // Krok 5-6: przesuń czas do najbliższego r
        t = N.top().GetR();
      } else {
        // Krok 7-8: wybierz zadanie o największym q
        Zadanie e = G.top();
        G.pop();

        t += e.GetP();
        Cmax = std::max(Cmax, t + e.GetQ());
      }
    }
    std::cout << "Schrage Cmax: " << Cmax << std::endl;
  }