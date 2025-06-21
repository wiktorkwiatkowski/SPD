#include "../inc/instance_loader.h"

#include <fstream>
#include <iostream>
#include <sstream>

FlowShopInstance InstanceLoader::loadFromFile(const std::string& filename) {
    FlowShopInstance instance;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Błąd: nie można otworzyć pliku " << filename << "\n";
        return instance;
    }

    file >> instance.num_jobs >> instance.num_machines;
    instance.processing_times.resize(instance.num_jobs, std::vector<int>(instance.num_machines));

    for (int i = 0; i < instance.num_jobs; ++i) {
        for (int j = 0; j < instance.num_machines; ++j) {
            int machine_index, time;
            file >> machine_index >> time;
            instance.processing_times[i][machine_index] = time;
        }
    }

    return instance;
}
