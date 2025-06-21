#ifndef GENERATOR_H
#define GENERATOR_H

#include <random>
#include <string>
#include <vector>

class Generator {
   private:
    int min_time;
    int max_time;
    std::mt19937 rng;

   public:
    Generator(int minTime = 1, int maxTime = 99);

    void generateInstance(int machines, int jobs, const std::string& filename);
    void generateMultipleInstances(const std::vector<int>& machine_counts,
                                   const std::vector<int>& job_counts,
                                   const std::string& prefix = "inst");
};

#endif  // GENERATOR_H
