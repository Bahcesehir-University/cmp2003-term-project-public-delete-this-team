#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <utility>
using namespace std;

struct ZoneCount {
    string zone;
    long long count = 0;
    long long hourCounts[24] = {0};
};

struct SlotCount {
    string zone;
    int hour;              // 0–23
    long long count = 0;
};

class TripAnalyzer {
private:
    unordered_map<string, ZoneCount> zoneMap;
public:
    // Parse Trips.csv, skip dirty rows, never crash
    void ingestFile(string csvPath);
    TripAnalyzer() {
        zoneMap.reserve(1000);
    }
    // Top K zones: count desc, zone asc
    vector<ZoneCount> topZones(int k = 10) const;

    // Top K slots: count desc, zone asc, hour asc
    vector<SlotCount> topBusySlots(int k = 10) const;
};






