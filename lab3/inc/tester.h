#ifndef TESTER_H
#define TESTER_H

#include "instance_loader.h"
#include <string>
#include <vector>

class Tester {
public:
    void runBruteForceTest(const std::vector<std::pair<std::string, FlowShopInstance>>& instances);
    std::string formatResult(int cmax, int bestCmax, long long time, bool isTimeMs = false) const;
    void saveToCSV(const std::string& filename,
                   const std::vector<std::vector<std::string>>& data) const;
};

#endif
