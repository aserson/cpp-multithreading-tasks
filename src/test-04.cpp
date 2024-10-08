﻿#include "task-04.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cassert>

void StringAnalysis(const std::string &output, std::vector<int> &eating_count, std::vector<int> &finish_count) {
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        for (int i = 0; i < eating_count.size(); i++) {
            std::string finding_line = "Philosopher " + std::to_string(i) + " is eating.";
            if (line.find(finding_line) != std::string::npos) {
                eating_count[i]++;
                break;
            }

            finding_line = "Philosopher " + std::to_string(i) + " finished eating.";
            if (line.find(finding_line) != std::string::npos) {
                finish_count[i]++;
                break;
            }
        }
    }
}

template<int PhilosopherCount>
void EatingCountTest(int expected, unsigned int time_for_thinking, unsigned int time_for_eating) {
    std::cout << "Eating " << PhilosopherCount << " Philosophers " << expected << " times... ";

    std::streambuf *original_buffer = std::cout.rdbuf();

    std::ostringstream captured_output;
    std::cout.rdbuf(captured_output.rdbuf());

    {
        Table<PhilosopherCount> table(time_for_thinking, time_for_eating);
        table.run(expected);
    }
    std::cout.rdbuf(original_buffer);

    std::vector<int> eating_count;
    std::vector<int> finish_count;
    eating_count.resize(PhilosopherCount, 0);
    finish_count.resize(PhilosopherCount, 0);

    StringAnalysis(captured_output.str(), eating_count, finish_count);

    assert(std::all_of(eating_count.begin(), eating_count.end(), [expected](const int elem) {
        return elem == expected;
        }));

    assert(std::all_of(finish_count.begin(), finish_count.end(), [expected](const int elem) {
        return elem == expected;
        }));

    std::cout << "Done!" << std::endl;
}

int main() {
    EatingCountTest<3>(3, 200, 200);
    EatingCountTest<4>(3, 200, 200);
    EatingCountTest<5>(3, 200, 200);
    EatingCountTest<6>(3, 200, 200);

    EatingCountTest<3>(4, 200, 200);
    EatingCountTest<4>(4, 200, 200);
    EatingCountTest<5>(4, 200, 200);
    EatingCountTest<6>(4, 200, 200);

    EatingCountTest<3>(5, 200, 200);
    EatingCountTest<4>(5, 200, 200);
    EatingCountTest<5>(5, 200, 200);
    EatingCountTest<6>(5, 200, 200);

    return 0;
}
