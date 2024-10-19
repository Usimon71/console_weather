#include "weather.h"

WeatherData::WeatherData(const Json& weather_data)
    : temps(weather_data["hourly"]["temperature_2m"])
    , winds(weather_data["hourly"]["wind_speed_10m"])
    , weather_codes(weather_data["hourly"]["weather_code"])
    , dates(weather_data["hourly"]["time"])
    {
    Json weather_types = JsonProcess::ParseJson("configs/weather_types.json");
    if (!JsonProcess::success_) {
        std::cerr << "Unable to open weather_types config\n";
    }
    std::vector<double> rounded_temps;
    double temp_sum = 0;
    for (size_t i = 0; i != temps.size(); ++i) {
        temp_sum += temps[i];
        if ((i + 1) % 6 == 0) {
            rounded_temps.push_back(std::round((temp_sum / 6) * 10) / 10.0);
            temp_sum = 0;
        }
    }
    temps = rounded_temps;

    std::vector<double> rounded_winds;
    temp_sum = 0;
    for (size_t i = 0; i != winds.size(); ++i) {
        temp_sum += winds[i];
        if ((i + 1) % 6 == 0) {
            rounded_winds.push_back(std::round((temp_sum / 6) * 10) / 36.0);
            temp_sum = 0;
        }
    }
    winds = rounded_winds;

    std::vector<int> weather_codes_sieved;
    for(size_t i = 2; i < weather_codes.size(); i += 6) {
        weather_codes_sieved.push_back(weather_codes[i]);
    }
    weather_codes = weather_codes_sieved;
}