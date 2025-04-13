#include "problem.hpp"



class Test {
    Problem inst;

    public:
    // Test dla pojedynczych instancji danego rozmiaru
    void runTest();
    // Test dla dużej ilości instancji, liczy średnie czasy i błędy
    void runAverageTest();
};