#ifndef INSTANCE_LOADER_H
#define INSTANCE_LOADER_H

#include <vector>
#include <string>

struct FlowShopInstance {
    int num_jobs;
    int num_machines;
    std::vector<std::vector<int>> processing_times; // [job][machine]
};

class InstanceLoader {
public:
    static FlowShopInstance loadFromFile(const std::string& filename);
};

#endif // INSTANCE_LOADER_H
