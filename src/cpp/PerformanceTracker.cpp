#include "PerformanceTracker.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#ifdef __unix__
#include <unistd.h>
#include <sys/resource.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <psapi.h>
#endif

using namespace std;
using namespace chrono;

PerformanceTracker::PerformanceTracker() : peakMemoryUsage(0) {}

void PerformanceTracker::startTimer(const string& operation) {
    timers[operation].startTime = high_resolution_clock::now();
}

void PerformanceTracker::endTimer(const string& operation) {
    auto& timer = timers[operation];
    auto endTime = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(endTime - timer.startTime).count();
    
    timer.totalDuration += duration / 1000.0; // Convert to milliseconds
    timer.callCount++;
}

double PerformanceTracker::getElapsedTime(const string& operation) const {
    auto it = timers.find(operation);
    if (it != timers.end()) {
        return it->second.totalDuration;
    }
    return 0.0;
}

void PerformanceTracker::recordMemoryUsage(const string& operation, size_t bytes) {
    memoryRecords[operation] = bytes;
    if (bytes > peakMemoryUsage) {
        peakMemoryUsage = bytes;
    }
}

size_t PerformanceTracker::getMemoryUsage(const string& operation) const {
    auto it = memoryRecords.find(operation);
    if (it != memoryRecords.end()) {
        return it->second;
    }
    return 0;
}

size_t PerformanceTracker::getPeakMemoryUsage() const {
    return peakMemoryUsage;
}

size_t PerformanceTracker::getCurrentMemoryUsage() const {
#ifdef __unix__
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        return usage.ru_maxrss * 1024; // ru_maxrss is in KB on Linux
    }
#elif defined(_WIN32) || defined(_WIN64)
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), 
                            (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.PrivateUsage;
    }
#endif
    return 0;
}

void PerformanceTracker::printReport() const {
    cout << "\n========== Performance Report ==========\n";
    cout << fixed << setprecision(3);
    
    cout << "\n--- Timing Statistics ---\n";
    for (const auto& pair : timers) {
        const string& operation = pair.first;
        const TimerInfo& timer = pair.second;
        
        double avgTime = timer.callCount > 0 ? 
            timer.totalDuration / timer.callCount : 0.0;
        
        cout << "Operation: " << operation << "\n";
        cout << "  Total Time: " << timer.totalDuration << " ms\n";
        cout << "  Calls: " << timer.callCount << "\n";
        cout << "  Average Time: " << avgTime << " ms\n";
        cout << "\n";
    }
    
    cout << "--- Memory Statistics ---\n";
    for (const auto& pair : memoryRecords) {
        cout << "Operation: " << pair.first << "\n";
        cout << "  Memory Usage: " << (pair.second / 1024.0 / 1024.0) << " MB\n";
        cout << "\n";
    }
    
    cout << "Peak Memory Usage: " << (peakMemoryUsage / 1024.0 / 1024.0) << " MB\n";
    cout << "Current Memory Usage: " << (getCurrentMemoryUsage() / 1024.0 / 1024.0) << " MB\n";
    cout << "========================================\n\n";
}

void PerformanceTracker::clear() {
    timers.clear();
    memoryRecords.clear();
    peakMemoryUsage = 0;
}

double PerformanceTracker::calculateElapsedTime(const TimerInfo& timer) const {
    auto currentTime = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(currentTime - timer.startTime).count();
    return duration / 1000.0;
}
