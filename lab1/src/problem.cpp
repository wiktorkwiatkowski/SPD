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
std::pair<int,int> Problem::Brute() {
  int minC = INT_MAX; // Inicjalizacja minimalnego Cmax
  std::vector<Zadanie> bestOrder;
  std::vector<Zadanie> perm = zadania;

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
  // std::cout << "Minimum Cmax dla wszystkich permutacji: " << minC << std::endl;
  // std::cout << "Rozwiazanie: ";
  // for (const auto &zadanie : bestOrder) {
  //   std::cout << zadanie.GetJ() << " ";
  // }
  // std::cout << std::endl;

  end = std::chrono::high_resolution_clock::now();
  int czas = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  return {minC, czas};

}
// Obliczanie Cmax po posortowaniu zadań według dostępności `r`
std::pair<int,int> Problem::SortR() {
  std::vector<Zadanie> perm = zadania;

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

  end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

  // std::cout << "Cmax dla sortowania po r: " << C << std::endl;
  return {C, duration.count()};
}

// Obliczanie Cmax po posortowaniu zadań według czasu stygnięcia `q`
std::pair<int,int> Problem::SortQ() {
  std::vector<Zadanie> perm = zadania;

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

  end = std::chrono::high_resolution_clock::now();
  int czas = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  // std::cout << "Cmax dla sortowania po q: " << C << std::endl;
  return {C, czas};
}

std::pair<int,int> Problem::Schrage() {

  // Lambda do sortowania N (min-heap po r)
  auto compareR = [](const Zadanie &a, const Zadanie &b) {
    return a.GetR() > b.GetR();
  };

  // Lambda do sortowania G (max-heap po q)
  auto compareQ = [](const Zadanie &a, const Zadanie &b) {
    return a.GetQ() < b.GetQ();
  };

  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();

  // Kolejka priorytetowa `N` przechowuje zadania nieuszeregowane, sortowane
  // według wartości `r` (czas dostępności zadania).
  // Jest to tzw. min-heap, czyli element o najmniejszym `r` znajduje się na
  // szczycie kolejki.
  //
  // `priority_queue` domyślnie działa jako max-heap (największy element na
  // szczycie). Odwracamy porządek sortowania(przez lambde compareR), aby
  // uzyskać min-heap: najmniejsze `r` na szczycie. Odrazu iniciujemy kolejke
  // zadaniami i sortujemy lamdą

  std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareR)> N(
      compareR, zadania);

  // Kolejka priorytetowa `G` przechowuje zadania gotowe do wykonania,
  // sortowane według `q` (czas dostarczenia zadania).
  // Jest to max-heap, czyli element o największym `q` znajduje się na
  // szczycie kolejki.
  //
  // Kolejka `G` jest początkowo pusta, ponieważ nie przekazujemy do niej
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

  end = std::chrono::high_resolution_clock::now();
  int czas = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  // std::cout << "Schrage Cmax: " << Cmax << std::endl;
  return {Cmax,czas};
}

std::pair<int,int> Problem::SchrageZPodzialem() {
  // Lambda do sortowania N (min-heap po r)
  auto compareR = [](const Zadanie &a, const Zadanie &b) {
      return a.GetR() > b.GetR();
  };

  // Lambda do sortowania G (max-heap po q)
  auto compareQ = [](const Zadanie &a, const Zadanie &b) {
      return a.GetQ() < b.GetQ();
  };

  std::chrono::high_resolution_clock::time_point start, end;
  start = std::chrono::high_resolution_clock::now();

  std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareR)> N(compareR, zadania);
  std::priority_queue<Zadanie, std::vector<Zadanie>, decltype(compareQ)> G(compareQ);

  int t = 0;
  int Cmax = 0;
  Zadanie current_task(-1, 0, 0, 0); // Dummy task
  bool has_current_task = false;

  while (!G.empty() || !N.empty()) {
      while (!N.empty() && N.top().GetR() <= t) {
          auto task = N.top();
          N.pop();

          if (has_current_task && task.GetQ() > current_task.GetQ()) {
              // Podziel zadanie
              int remaining_p = current_task.GetP() - (t - task.GetR());
              current_task.SetP(remaining_p);
              G.push(current_task);
              current_task = task;
              t = task.GetR();
          }
          G.push(task);
      }

      if (G.empty()) {
          t = N.top().GetR();
          continue;
      }

      auto task = G.top();
      G.pop();

      if (has_current_task && task.GetQ() > current_task.GetQ()) {
          // Podziel bieżące zadanie
          int remaining_p = current_task.GetP() - (t - task.GetR());
          current_task.SetP(remaining_p);
          G.push(current_task);
      }

      current_task = task;
      has_current_task = true;

      int next_r = N.empty() ? INT_MAX : N.top().GetR();
      int processing_time = std::min(current_task.GetP(), next_r - t);

      t += processing_time;
      current_task.SetP(current_task.GetP() - processing_time);

      if (current_task.GetP() == 0) {
          has_current_task = false;
          Cmax = std::max(Cmax, t + current_task.GetQ());
      } else {
          G.push(current_task);
      }
  }

  end = std::chrono::high_resolution_clock::now();
  int czas = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  return {Cmax, czas};
}









// Algorytm własny - priorytetowy(zadania, w_r, w_q, w_p):
//     1. Nieprzetworzone = lista wszystkich zadań
//     2. t = 0  # aktualny czas
//     3. π = [] # kolejność zadań
//     4. Dopóki Nieprzetworzone ≠ ∅:
//        a. Dla każdego zadania j w Nieprzetworzone:
//           - Oblicz Priority(j) = w_r * r_j + w_q * q_j + w_p * p_j
//        b. Wybierz zadanie j z NajMNIEJSZYM Priority(j) i r_j ≤ t
//        c. Jeśli brak zadań gotowych:
//           - t = min{r_j | j ∈ Nieprzetworzone}  # przeskocz w czasie
//        d. W przeciwnym razie:
//           - Dodaj j do π
//           - Usuń j z Nieprzetworzone
//           - t = t + p_j
//     5. Oblicz C_max(π) i zwróć wynik


std::pair<int,int> Problem::Wlasny() {

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
              float priority = w_r * remaining_tasks[i].GetR() + 
                             w_q * remaining_tasks[i].GetQ() + 
                             w_p * remaining_tasks[i].GetP();
              ready_tasks.emplace_back(priority, i);
          }
      }

      if (ready_tasks.empty()) {
          // Brak gotowych zadań - znajdź minimalne r_j
          auto min_r_task = std::min_element(
              remaining_tasks.begin(),
              remaining_tasks.end(),
              [](const Zadanie& a, const Zadanie& b) {
                  return a.GetR() < b.GetR();
              });
          t = min_r_task->GetR();
      } else {
          // Wybierz zadanie z najniższym priorytetem
          auto next_task = std::min_element(
              ready_tasks.begin(),
              ready_tasks.end(),
              [](const auto& a, const auto& b) {
                  return a.first < b.first;
              });

          Zadanie chosen = remaining_tasks[next_task->second];
          schedule.push_back(chosen);
          remaining_tasks.erase(remaining_tasks.begin() + next_task->second);

          // Aktualizuj czas i C_max
          t = std::max(t, chosen.GetR()) + chosen.GetP();
          C_max = std::max(C_max, t + chosen.GetQ());
      }
  }

  end = std::chrono::high_resolution_clock::now();
  int czas = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  return {C_max, czas};
}