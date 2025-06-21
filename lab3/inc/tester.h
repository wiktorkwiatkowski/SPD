#pragma once

#include <string>
#include <vector>

#include "../inc/instance_loader.h"
#include "../inc/scheduler.h"

class Tester {
   public:
    void runBruteForceAndNEH(
        const std::vector<std::pair<std::string, FlowShopInstance>>& instances);

   private:
    Scheduler scheduler;

    std::string formatResult(int cmax, int bestCmax, long long time, bool isTimeMs = false) const;
    void saveToCSV(const std::string& filename,
                   const std::vector<std::vector<std::string>>& data) const;
};
