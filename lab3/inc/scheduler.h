#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "instance_loader.h"
#include <utility>
#include <vector>

class Scheduler {
public:
    static std::pair<int, long long> bruteForce(const FlowShopInstance& instance);
};

#endif
