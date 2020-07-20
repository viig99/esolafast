//
// Created by Arjun Variar on 20/07/20.
//

#include <chrono>
#include <iostream>

class Timer {
    std::chrono::high_resolution_clock::time_point begin;
    std::chrono::high_resolution_clock::time_point end;
    std::string name;

public:
    Timer(std::string timer_name) {
        begin = std::chrono::high_resolution_clock::now();
        name = timer_name;
    }

    ~Timer() {
        end = std::chrono::high_resolution_clock::now();
        std::cout << "Time taken by " << name << " = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
    }
};