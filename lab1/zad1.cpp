/*
l|rj,qj|Cmax
n zadań j={1,...,j,n}
1 procesor
pj- czas wykonywania zadania j
rj - termin dostępności zadania j, czyli kiedy można zacząć wykonywać zadanie j to są kolejnośći w PI
qj - czas stygnięcia zadania j,
Cmax - długość uszeregowania, czas wykonywania wszystkich zadań - TĄ wartość chcemy ZMINIMALIZOWAC
czas stygnięcia nie wpływa na długość wkonywania się 
*/
/* 1 zadanie polega na wygenerowaniu takiej instancji jak na tablicy, funkcja liczaca kryterium sprawdzić,
 czy ta funkcja licząca dobrze działa, napisać sortowanie(f. wbudowane), użyć f. next perm mutation jakieś leksykograficzne wszystkie permutacje np.
generowanie permutacji w porządku leksykograficznym

1. wygenerować instację
2. wygenerwoiać f. liczącą kryterium
3. zrobić przegląd zupełny (ma sprawdzić 120 coś)
4. losowe instancje
5. czytać instancje losowe, do których będziemy mieli link jak liczymy błedy to do najlepszej optymalizacji 
nasz wynik przez to najlepsze * 100%
P pi(2)- czas szeregowania na pozycji drugiej
najpierw bez ogonków 
C pi(2) = r pi(1) + p pi(1)
C pi(2) = max{r pi(2), C pi(1)} + p pi(2)
C pi(j) = max{r pi{j}, C pi{j-1}+ p pi(j)}

jeszcze jakieś do liczenia
C qPI(j) = C PI(1) + qPI(1)
C qPI(j) = C PI(j) + q PI(j)

na 3.0 przegląd zupełny i  sortowanie po rj i qj
3.5 algorytm konstrukcyjny własnego autorstwa jakieś wagi coś dodać (np. stosunek rj do qj)
4.0 - Szrage 
4.5 - Szrage z zupełnymi

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
C PI(2) = max{r PI(2), c PI(2)} + p PI(2) = 2 + 5=7 -> c qPI(2) = 7+5(czas stygnięcia?)=12

Wartością kryterium jest maksymalna z tych wartośći q PI(j)
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

int j[5] = {1, 2, 3, 4, 5};
int p[5] = {2, 5, 1, 1, 2};
int r[5] = {5, 2, 3, 0, 6};
int q[5] = {2, 7, 1, 3, 1};

class Zadanie {
    int j, p, r, q;

public:
    Zadanie(int j, int p, int r, int q) : j(j), p(p), r(r), q(q) {}

    int GetJ() const { return j; }
    int GetP() const { return p; }
    int GetR() const { return r; }
    int GetQ() const { return q; }
};

class Problem {
    std::vector<Zadanie> zadania;
    std::vector<int> rozwiazanie;

public:
    void WczytajZPliku(const std::string& nazwaPliku) {
        std::ifstream plik(nazwaPliku);
        if (!plik) {
            std::cerr << "Nie mozna otworzyc pliku!" << std::endl;
            return;
        }

        int n;
        plik >> n; // Wczytaj liczbę zadań
        zadania.clear();

        for (int i = 0; i < n; i++) {
            int p, r, q;
            plik >> p >> r >> q;
            zadania.emplace_back(i+1, p, r, q);
        }
        plik.close();
    }

    void WyswietlInstancje() {
        for (const auto& zadanie : zadania) {
            std::cout << "j: " << zadanie.GetJ()
                      << " p: " << zadanie.GetP()
                      << " r: " << zadanie.GetR()
                      << " q: " << zadanie.GetQ() << std::endl;
        }
    }

    int PoliczCmax() {

        int minC = INT_MAX;
        rozwiazanie = std::vector<int>(zadania.size());

        do {
            int C = zadania[0].GetR() + zadania[0].GetP();
            for (size_t i = 1; i < zadania.size(); i++) {
                C = std::max(C, zadania[i].GetR()) + zadania[i].GetP();
            }
            C += zadania.back().GetQ();

            if(minC>C) {
                minC=C;
                rozwiazanie.clear();
                for (auto i : zadania) {
                    rozwiazanie.push_back(i.GetJ());
                }
            }

        } while (std::next_permutation(zadania.begin(), zadania.end(), [](const Zadanie& a, const Zadanie& b) {
            return a.GetJ() < b.GetJ();
        }));
        std::cout << "Minimum Cmax: " << minC << std::endl;
        std::cout << "Rozwiazanie: ";
        for (auto i : rozwiazanie) {
            std::cout << i << " ";
        }
        return minC;
    }
};

int main() {

    Problem inst;

    inst.WczytajZPliku("../SCHRAGE1.DAT");
    inst.WyswietlInstancje();
    inst.PoliczCmax();

    return 0;
}

