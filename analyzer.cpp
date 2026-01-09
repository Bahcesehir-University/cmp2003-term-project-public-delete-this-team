#include "analyzer.h"
#include <filesystem>
// Students may use ANY data structure internally

void TripAnalyzer::ingestFile(string csvPath) {
  
    
    ifstream file(csvPath);
    if (!file.is_open()) return;
    if(std::filesystem::is_empty(csvPath)) return;
    string line;
    getline(file,line);
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        line.reserve(256);
     
        if (line.empty()) continue;

        

        const char* charPtr = line.c_str();
        int commaCount = 0;

        const char* col2_start = nullptr;
        const char* col2_end = nullptr;
        const char* col4_start = nullptr;
        const char* col4_end = nullptr;

        while (*charPtr) {
            if (*charPtr == ',') {
                commaCount++;
                if (commaCount == 1) col2_start = charPtr + 1;
                else if (commaCount == 2) col2_end = charPtr;
                else if (commaCount == 3) col4_start = charPtr + 1;
                else if (commaCount == 4) col4_end = charPtr;
            }
            charPtr++;
        }

        if (commaCount < 5) continue;
        if (!col2_start || !col2_end || !col4_start || !col4_end) continue;
        if (col2_end <= col2_start || col4_end <= col4_start) continue;

        string zone(col2_start, col2_end - col2_start);
        if (zone.empty()) continue;

        const char* dateTimePtr = col4_start;
        const char* dateTimeEnd = col4_end;

        while (dateTimePtr < dateTimeEnd && *dateTimePtr != ' ')
            dateTimePtr++;

        if (dateTimePtr >= dateTimeEnd) continue;
        dateTimePtr++;

        if (dateTimePtr + 1 >= dateTimeEnd) continue;
        if (dateTimePtr[0] < '0' || dateTimePtr[0] > '9') continue;
        if (dateTimePtr[1] < '0' || dateTimePtr[1] > '9') continue;

        int hour = (dateTimePtr[0] - '0') * 10 + (dateTimePtr[1] - '0');
        if (hour < 0 || hour >= 24) continue;

        auto& zoneData = zoneMap[zone];
        if (zoneData.zone.empty()) {
            zoneData.zone = zone;
        }
        zoneData.count++;
        zoneData.hourCounts[hour]++;
    }
    file.close();
}

vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    vector<ZoneCount> result;
    result.reserve(zoneMap.size());

    for (const auto& p : zoneMap)
        result.push_back(p.second);

    auto sorter = [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
    };

    if (result.size() > (size_t)k) {
        partial_sort(result.begin(), result.begin() + k, result.end(), sorter);
        result.resize(k);
    } else {
        sort(result.begin(), result.end(), sorter);
    }

    return result;
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    vector<SlotCount> result;
    result.reserve(zoneMap.size() * 24);

    for (const auto& p : zoneMap) {
        const auto& zoneData = p.second;
        for (int h = 0; h < 24; ++h) {
            if (zoneData.hourCounts[h] > 0) {
                SlotCount sc;
                sc.zone = zoneData.zone;
                sc.hour = h;
                sc.count = zoneData.hourCounts[h];
                result.push_back(sc);
            }
        }
    }

    auto sorter = [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
    };

    if (result.size() > (size_t)k) {
        partial_sort(result.begin(), result.begin() + k, result.end(), sorter);
        result.resize(k);
    } else {
        sort(result.begin(), result.end(), sorter);
    }

    return result;
}





