#include "weather.h"

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/captured_mouse.hpp" 
#include "ftxui/component/component.hpp"       
#include "ftxui/component/event.hpp" 
#include "cpr/cpr.h"

using namespace ftxui;

struct WeatherRequestParams {
    double latitude;
    double longitude;
    uint8_t forecast_days;
};

struct RunData {
    std::vector<std::string> cities;
    size_t frequency;
    uint8_t forecast_days;
};

class Interface {
public:
    Interface();
    void RunInterface();
    ~Interface() = default;
private:
    int RequestWeather(size_t ind);
    void GetWRP(size_t ind);
    std::string GetApiKey();
    Component GetRenderer() const;
    Component GetComponent(Component& renderer, ScreenInteractive& screen);

    WeatherRequestParams wrp_; // latitude, longitude, forecast_days
    WeatherData weather_data_; // temps, winds, weather codes, weather types
    RunData run_data_; // cities list, forecast_days, frequency_
    Json cities_data_;
    Json months_;
    Json weather_types_;
    int64_t current_city_ind_; 
    size_t cities_count_;

};