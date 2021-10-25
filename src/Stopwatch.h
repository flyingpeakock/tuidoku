#pragma once
#include <thread>
#include <string>

class Stopwatch {
    private:
        bool running;
        std::thread counter;
        int milliseconds, seconds, minutes, hours, days, weeks;
    public:
        void count();
        Stopwatch();
        Stopwatch(const Stopwatch &boj);
        Stopwatch &operator=(const Stopwatch &obj);
        void start();
        void stop();
        std::string timeTaken();
        int totalSeconds();
        void reset();
};