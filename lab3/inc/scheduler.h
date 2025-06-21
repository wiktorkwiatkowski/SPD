#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <utility>
#include <vector>

#include "instance_loader.h"

class Scheduler {
   public:
    static std::pair<int, long long> bruteForce(const FlowShopInstance& instance);
    static std::pair<int, long long> calculateNEH(const FlowShopInstance& instance);
    static std::pair<int, long long> johnson(const FlowShopInstance& instance);
    static std::pair<int, long long> calculateFNEH(const FlowShopInstance& instance);
};

#endif
