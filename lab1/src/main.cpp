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

#include "../include/problem.hpp"


// Główna funkcja programu
int main() {
  Problem inst;

  // Wczytaj instancję z pliku
  inst.WczytajZPliku("../SCHRAGE2.DAT");

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