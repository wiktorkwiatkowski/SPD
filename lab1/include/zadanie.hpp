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

    void SetP(int p) { this->p = p; }
    void SetQ(int q) { this->q = q; }
  
    // Operator porównania dla sortowania (sortuje rosnąco po j)
    bool operator<(const Zadanie &other) const { return j < other.j; }
  };