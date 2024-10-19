#pragma once

#include "json.h"

#include <cstdint>
#include <vector>
#include <string>

class WeatherData {
public:
    WeatherData(const Json& weather_data);
        
    WeatherData() = default;
    ~WeatherData() = default;

    std::vector<double> temps;
    std::vector<double> winds;
    std::vector<int> weather_codes;
    std::vector<std::string> dates;
    Json weather_types;
};