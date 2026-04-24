#ifndef PERFORMANCE_TRACKER_H
#define PERFORMANCE_TRACKER_H

#include <chrono>
#include <string>
#include <vector>
#include <map>

using namespace std;

class PerformanceTracker {
public:
    PerformanceTracker();
    
    void startTimer(const string& operation);
    
    void endTimer(const string& operation);
    
    double getElapsedTime(const string& operation) const;
    
    void recordMemoryUsage(const string& operation, size_t bytes);
    
    size_t getMemoryUsage(const string& operation) const;
    
    size_t getPeakMemoryUsage() const;
    
    void printReport() const;
    
    void clear();
    
    size_t getCurrentMemoryUsage() const;
    
private:
    struct TimerInfo {
        chrono::high_resolution_clock::time_point startTime;
        double totalDuration; 
        int callCount;
        
        TimerInfo() : totalDuration(0.0), callCount(0) {}
    };
    
    map<string, TimerInfo> timers;
    map<string, size_t> memoryRecords;
    size_t peakMemoryUsage;
    
    double calculateElapsedTime(const TimerInfo& timer) const;
};

#endif 
