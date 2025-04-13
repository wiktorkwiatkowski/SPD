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

// Funkcja licząca Cmax dla wszystkich możliwych permutacji (przegląd zupełny -
// brute force)
std::pair<int, int> Problem::Brute() {
  int minC = INT_MAX; // Inicjalizacja minimalnego Cmax
  std::vector<Zadanie> bestOrder;
  std::vector<Zadanie> perm = zadania;

  // Start pomiaru czasu
  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();

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
  // std::cout << "Minimum Cmax dla wszystkich permutacji: " << minC <<
  // std::endl; std::cout << "Rozwiazanie: "; for (const auto &zadanie :
  // bestOrder) {
  //   std::cout << zadanie.GetJ() << " ";
  // }
  // std::cout << std::endl;

  // koniec pomiaru czasu
  end = std::chrono::high_resolution_clock::now();
  int czas = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                 .count();

  return {minC, czas};
}

// Obliczanie Cmax po posortowaniu zadań według dostępności `r`
std::pair<int, int> Problem::SortR() {
  std::vector<Zadanie> perm = zadania;

  // Start pomiaru czasu
  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();

  std::sort(perm.begin(), perm.end(), [](const Zadanie &a, const Zadanie &b) {
    return a.GetR() < b.GetR();
  });

  int C = 0, actual_time = 0;
  for (const auto &zadanie : perm) {
    actual_time = std::max(actual_time, zadanie.GetR()) + zadanie.GetP();
    C = std::max(C, actual_time + zadanie.GetQ());
  }

  // Koniec pomiaru czasu
  end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

  // std::cout << "Cmax dla sortowania po r: " << C << std::endl;
  return {C, duration.count()};
}

// Obliczanie Cmax po posortowaniu zadań według czasu stygnięcia `q`
std::pair<int, int> Problem::SortQ() {
  std::vector<Zadanie> perm = zadania;

  // Start pomiaru czasu
  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();

  std::sort(perm.begin(), perm.end(), [](const Zadanie &a, const Zadanie &b) {
    return a.GetQ() > b.GetQ();
  });

  int C = 0, actual_time = 0;
  for (const auto &zadanie : perm) {
    actual_time = std::max(actual_time, zadanie.GetR()) + zadanie.GetP();
    C = std::max(C, actual_time + zadanie.GetQ());
  }

  // Koniec pomiaru czasu
  end = std::chrono::high_resolution_clock::now();
  int czas =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  // std::cout << "Cmax dla sortowania po q: " << C << std::endl;
  return {C, czas};
}

std::pair<int, int> Problem::Schrage() {

  // Lambda do sortowania N (kolejka min-heap po r)
  auto compareR = [](const Zadanie &a, const Zadanie &b) {
    return a.GetR() > b.GetR();
  };

  // Lambda do sortowania G (kolejka max-heap po q)
  auto compareQ = [](const Zadanie &a, const Zadanie &b) {
    return a.GetQ() < b.GetQ();
  };

  // Start pomiaru czasu
  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();

  // Kolejka priorytetowa N przechowuje zadania nieuszeregowane, sortowane
  // według wartości r (czas dostępności zadania).
  // Jest to tzw. min-heap, czyli element o najmniejszym r znajduje się na
  // szczycie kolejki.
  //
  // priority_queue domyślnie działa jako max-heap (największy element na
  // szczycie). Odwracamy porządek sortowania(przez lambde compareR), aby
  // uzyskać min-heap: najmniejsze r na szczycie. Odrazu iniciujemy kolejke
  // zadaniami i sortujemy lamdą

  std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareR)> N(
      compareR, zadania);

  // Kolejka priorytetowa G przechowuje zadania gotowe do wykonania,
  // sortowane według q (czas dostarczenia zadania).
  // Jest to max-heap, czyli element o największym q znajduje się na
  // szczycie kolejki.
  //
  // Kolejka G jest początkowo pusta, ponieważ nie przekazujemy do niej
  // żadnych danych przy inicjalizacji.
  // Zadania będą dodawane dynamicznie podczas działania algorytmu Schrage.
  //
  std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareQ)> G(
      compareQ);

  int t = 0;    // Aktualny czas
  int Cmax = 0; // Maksymalny czas dostarczenia

  // Pętla działa tak długo, jak długo istnieją jakiekolwiek zadania do
  // przetworzenia, czy to w zbiorze zadań nieuszeregowanych (N), czy zadań
  // gotowych do realizacji (G)
  while (!G.empty() || !N.empty()) {

    // Sprawdzenie, czy są jeszcze jakieś zadania w N i czy zadanie na szczycie
    // kolejki ma r (ready) mniejsze lub równe aktualnemu czasowi t
    // Warunek jest powtarzany do momenty, az wszystkie zadania spełniające
    // warunek r<=t zostaną przeniesione
    while (!N.empty() && N.top().GetR() <= t) {
      G.push(N.top());
      N.pop();
    }

    // Jeżeli G jest puste oznacza to, że nie ma żadnego dostępnego zadania do
    // wykonania Wymusza to, że trzeba czekać, aż jakieś zadanie stanie się
    // dostępne. Ustawiamy t na najbliższe r z kolejki N, czyli pierwsze
    // dostępne zadanie.
    if (G.empty()) {
      t = N.top().GetR();
    } else {
      // Jeśli kolejka nie jest pusta to pobieramy zadanie o największym q z G
      // oraz usuwamy bo będzie teraz przetwarzane
      Zadanie e = G.top();
      G.pop();

      // Aktualizacja czasu, czas zwiększa się o czas p (process) przetwarzania
      // zadania
      t += e.GetP();
      // Maksymalny czas dostarczenia aktualizauje się na większą z dwóch
      // wartości:
      // - obecna wartość cmax
      // - t e.GetQ(), czyli momemnt zakończenia zadania + jego q (czas
      // stygnięcia)
      Cmax = std::max(Cmax, t + e.GetQ());
    }
  }

  // Koniec pomiaru czasu
  end = std::chrono::high_resolution_clock::now();
  int czas =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  // std::cout << "Schrage Cmax: " << Cmax << std::endl;
  return {Cmax, czas};
}

// Algorytm Schrage z podziałem zadan
std::pair<int, int> Problem::SchrageZPodzialem() {
  // Lambda do sortowania N (kolejka min-heap po r)
  // Porównuje dwa zadania po wartości r (czas dostępności zadania).
  // Zwraca true, jeśli r pierwszego zadania jest większe niż drugiego,
  // co skutkuje min-heap (najmniejsze r na szczycie kolejki).
  auto compareR = [](const Zadanie &a, const Zadanie &b) {
    return a.GetR() > b.GetR();
  };

  // Lambda do sortowania G (kolejka max-heap po q)
  // Porównuje dwa zadania po wartości q (czas dostarczenia zadania).
  // Zwraca true, jeśli q pierwszego zadania jest mniejsze niż drugiego,
  // co skutkuje max-heap (największe q na szczycie kolejki).
  auto compareQ = [](const Zadanie &a, const Zadanie &b) {
    return a.GetQ() < b.GetQ();
  };

  // Inicjalizacja kolejki N jako min-heap po r.
  // Przechowuje zadania nieuszeregowane, posortowane rosnąco według r`.
  // Kolejka jest od razu wypełniona wszystkimi zadaniami z listy zadania.
  std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareR)> N(
      compareR, zadania);

  // Inicjalizacja kolejki G jako max-heap po q.
  // Przechowuje zadania gotowe do wykonania, posortowane malejąco według q.
  // Kolejka jest początkowo pusta, zadania są dodawane dynamicznie.
  std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareQ)> G(
      compareQ);

  int t = 0;    // Aktualny czas symulacji
  int Cmax = 0; // Maksymalny czas dostarczenia

  // Zmienna przechowująca aktualnie przetwarzane zadanie.
  // Inicjalizacja wartościami domyślnymi (p = 0, q = INT_MAX),
  // aby uniknąć nieoczekiwanych zachowań na początku algorytmu.
  Zadanie aktualneZadanie(0, 0, 0, 0);
  aktualneZadanie.SetP(0);
  aktualneZadanie.SetQ(INT_MAX); // Wartość startowa do porównań

  // Rozpoczęcie pomiaru czasu wykonania algorytmu
  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();

  // Główna pętla algorytmu - działa dopóki są zadania do przetworzenia
  // (w kolejce N lub G).
  while (!G.empty() || !N.empty()) {
    // Aktualizacja zbioru zadań gotowych do wykonania (G).
    // Przenoszenie zadań z kolejki N do G, jeśli ich czas dostępności r
    // jest mniejszy lub równy aktualnemu czasowi t.
    while (!N.empty() && N.top().GetR() <= t) {
      Zadanie j = N.top();
      N.pop();
      G.push(j);

      // Sprawdzenie, czy nowo dodane zadanie ma większe q niż aktualnie
      // przetwarzane zadanie. Jeśli tak, następuje przerwanie przetwarzania.
      if (j.GetQ() > aktualneZadanie.GetQ()) {
        // Obliczenie pozostałego czasu przetwarzania aktualnego zadania
        // i ustawienie go jako nowego czasu przetwarzania.
        aktualneZadanie.SetP(t - j.GetR());
        t = j.GetR(); // Cofnięcie czasu do momentu dostępności nowego zadania

        // Jeśli pozostały czas przetwarzania jest większy od zera,
        // zadanie wraca do kolejki G.
        if (aktualneZadanie.GetP() > 0) {
          G.push(aktualneZadanie);
        }
      }
    }

    // Jeśli kolejka G jest pusta, oznacza to, że nie ma zadań gotowych.
    // Czekamy na najbliższe dostępne zadanie, ustawiając czas t na jego r.
    if (G.empty()) {
      t = N.top().GetR();
    } else {
      // Pobranie zadania o największym q z kolejki G.
      Zadanie j = G.top();
      G.pop();
      aktualneZadanie = j; // Rozpoczęcie przetwarzania nowego zadania

      // Aktualizacja czasu o czas przetwarzania zadania.
      t += j.GetP();

      // Aktualizacja maksymalnego czasu dostarczenia.
      // Nowa wartość to maksimum z dotychczasowego Cmax i t + q (czas
      // zakończenia + czas dostarczenia).
      Cmax = std::max(Cmax, t + j.GetQ());
    }
  }

  // Zakończenie pomiaru czasu i obliczenie czasu wykonania w nanosekundach.
  end = std::chrono::high_resolution_clock::now();
  int czas =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  // Zwrócenie pary wartości: maksymalnego czasu dostarczenia (Cmax)
  // oraz czasu wykonania algorytmu w nanosekundach.
  return {Cmax, czas};
}

std::pair<int, int> Problem::Wlasny() {

  float w_r = 0.3f;
  float w_q = 0.7f;
  float w_p = -0.2f;

  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();

  std::vector<Zadanie> remaining_tasks = zadania;
  std::vector<Zadanie> schedule;
  int t = 0, C_max = 0;

  while (!remaining_tasks.empty()) {
    // Znajdź zadania gotowe do wykonania (r_j <= t)
    std::vector<std::pair<float, size_t>> ready_tasks;
    for (size_t i = 0; i < remaining_tasks.size(); ++i) {
      if (remaining_tasks[i].GetR() <= t) {
        // Oblicz priorytet
        float priority = w_r * remaining_tasks[i].GetR() + w_q * remaining_tasks[i].GetQ() + w_p * remaining_tasks[i].GetP();
        ready_tasks.emplace_back(priority, i);
      }
    }

    if (ready_tasks.empty()) {
      // Brak gotowych zadań - znajdź minimalne r_j
      auto min_r_task = std::min_element(remaining_tasks.begin(), remaining_tasks.end(), [](const Zadanie &a, const Zadanie &b) {return a.GetR() < b.GetR();});
      t = min_r_task->GetR();
    } else {
      // Wybierz zadanie z najniższym priorytetem
      auto next_task = std::min_element(ready_tasks.begin(), ready_tasks.end(), [](const auto &a, const auto &b) { return a.first < b.first; });

      Zadanie chosen = remaining_tasks[next_task->second];
      schedule.push_back(chosen);
      remaining_tasks.erase(remaining_tasks.begin() + next_task->second);

      // Aktualizuj czas i C_max
      t = std::max(t, chosen.GetR()) + chosen.GetP();
      C_max = std::max(C_max, t + chosen.GetQ());
    }
  }

  end = std::chrono::high_resolution_clock::now();
  int czas =  std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  return {C_max, czas};
}