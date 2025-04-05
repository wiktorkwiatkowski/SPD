/*
l|rj,qj|Cmax
n zadań j={1,...,j,n}
1 procesor
pj- czas wykonywania zadania j
rj - termin dostępności zadania j, czyli kiedy można zacząć wykonywać zadanie j
to są kolejnośći w PI qj - czas stygnięcia zadania j, Cmax - długość
uszeregowania, czas wykonywania wszystkich zadań - TĄ wartość chcemy
ZMINIMALIZOWAC czas stygnięcia nie wpływa na długość wkonywania się 
*/
/* 1 zadanie polega na wygenerowaniu takiej instancji jak na tablicy, funkcja
liczaca kryterium sprawdzić, czy ta funkcja licząca dobrze działa, napisać
sortowanie(f. wbudowane), użyć f. next perm mutation jakieś leksykograficzne
wszystkie permutacje np. generowanie permutacji w porządku leksykograficznym

1. wygenerować instację
2. wygenerwoiać f. liczącą kryterium
3. zrobić przegląd zupełny (ma sprawdzić 120 coś)
4. losowe instancje
5. czytać instancje losowe, do których będziemy mieli link jak liczymy błedy to
do najlepszej optymalizacji nasz wynik przez to najlepsze * 100% P pi(2)- czas
szeregowania na pozycji drugiej najpierw bez ogonków C pi(2) = r pi(1) + p pi(1)
C pi(2) = max{r pi(2), C pi(1)} + p pi(2)
C pi(j) = max{r pi{j}, C pi{j-1}+ p pi(j)}

jeszcze jakieś do liczenia
C qPI(j) = C PI(1) + qPI(1)
C qPI(j) = C PI(j) + q PI(j)

na 3.0 przegląd zupełny i  sortowanie po rj i qj
3.5 algorytm konstrukcyjny własnego autorstwa jakieś wagi coś dodać (np.
stosunek rj do qj) 4.0 - Szrage 4.5 - Szrage z zupełnymi

na kolejne zajęcia 31.03 zrobić jakiś szkielet

przegląd zupełny do 11,12 zadań maks!


Zrobić klasę zadanie
int PI;
int rj;
int qj;

i do tego klasę Problem
vector<int> rozwiązanie;
int Kryterium()
i to ma być na kolejne zajęcia!!! czyli 31.03

Jakaś nowa instancja
C pi(1) = r PI(1) + p PI(1) = 1 -> Cq PI(1) = 1+3=4
C PI(2) = max{r PI(2), c PI(2)} + p PI(2) = 2 + 5=7 -> c qPI(2) = 7+5(czas
stygnięcia?)=12

Wartością kryterium jest maksymalna z tych wartośći q PI(j)
*/

/*
na 4 i na 4.5 Schrage
Zaczynamy od zadania o najmniejszym Rj - najwcześniejszy termin dostępności,
który ma najdłuższy ogonek(priorytet) sortujemy po Rj w pierwszym ruchu i potem
trafia do kolejki priorytetowej
*/

/*Schrage z podziałem różni */

#include <algorithm>
#include <chrono>
#include <climits>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

int j[5] = {1, 2, 3, 4, 5};
int p[5] = {2, 5, 1, 1, 2};
int r[5] = {5, 2, 3, 0, 6};
int q[5] = {2, 7, 1, 3, 1};

// Klasa Zadanie przechowuje informacje o zadaniu (j - numer, r - dostępność, p
// - czas trwania, q - stygnięcie)
class Zadanie {
  int j, p, r, q;

public:
  // Konstruktor inicjalizujący zmienne
  Zadanie(int j, int r, int p, int q) : j(j), p(p), r(r), q(q) {}

  // Gettery do pobierania wartości
  int GetJ() const { return j; }
  int GetP() const { return p; }
  int GetR() const { return r; }
  int GetQ() const { return q; }

  // Operator porównania dla sortowania (sortuje rosnąco po j)
  bool operator<(const Zadanie &other) const { return j < other.j; }
};

// Klasa Problem przechowuje zadania i implementuje różne algorytmy
class Problem {
  std::vector<Zadanie> zadania; // Wektor zadań

public:
  // Funkcja wczytująca dane z pliku
  void WczytajZPliku(const std::string &nazwaPliku) {
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
  void WyswietlInstancje() {
    for (const auto &zadanie : zadania) {
      std::cout << "j: " << zadanie.GetJ() << " | r: " << zadanie.GetR()
                << " p: " << zadanie.GetP() << " q: " << zadanie.GetQ()
                << std::endl;
    }
    std::cout << "============================================" << std::endl;
  }

  // Funkcja licząca Cmax dla wszystkich możliwych permutacji (przegląd zupełny)
  int PoliczCmax() {
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
  int PoliczCmaxSortR() {
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
  int PoliczCmaxSortQ() {
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

  /*===============================================================================================================================================*/
  /*===============================================================================================================================================*/
  /*===============================================================================================================================================*/

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
  void Schrage() {
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
};

// Główna funkcja programu
int main() {
  Problem inst;

  // Wczytaj instancję z pliku
  inst.WczytajZPliku("../SCHRAGE1.DAT");

  // Wyświetl zadania
  inst.WyswietlInstancje();

  // Oblicz Cmax dla sortowania po r
  inst.PoliczCmaxSortR();

  // Oblicz Cmax dla sortowania po q
  inst.PoliczCmaxSortQ();

  // Oblicz minimalne Cmax poprzez przegląd zupełny (wszystkie permutacje)
  inst.PoliczCmax();

  inst.Schrage();

  return 0;
}