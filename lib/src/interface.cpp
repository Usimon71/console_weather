#include "interface.h"
#include <atomic>
#include <thread>

struct ColorRGB {
    ColorRGB(uint8_t r, uint8_t g, uint8_t b)
        : red_(r),
          green_(g),
          blue_(b)
    {}
    ColorRGB() = default;
    uint8_t red_;
    uint8_t green_;
    uint8_t blue_;
};

Interface::Interface()
        : current_city_ind_(0)
    {
        Json run_data = JsonProcess::ParseJson("configs/my_config.json");
        if (!JsonProcess::success_) {
            std::cerr << "Unable to open run config file\n";
        }
        run_data_.cities = std::vector<std::string>(run_data["cities"]);
        run_data_.forecast_days = run_data["forecast_days"];
        run_data_.frequency = run_data["frequency"];

        cities_count_ = run_data_.cities.size();

        wrp_.forecast_days = run_data_.forecast_days;

        weather_types_ = JsonProcess::ParseJson("configs/weather_types.json");
        if (!JsonProcess::success_) {
            std::cerr << "Unable to open weather_types config file\n";
        }

        Json array_cities = {};
        for (size_t i = 0; i != run_data_.cities.size(); ++i) {
            cpr::Response r_city = cpr::Get(cpr::Url{"https://api.api-ninjas.com/v1/city"},
                                cpr::Header{{"X-Api-Key", GetApiKey()}},
                                cpr::Parameters{{"name", run_data_.cities[i]}});
            Json city_data {};
            if (r_city.status_code == 200) {
                city_data = Json::parse(r_city.text);
                array_cities.push_back(city_data[0]);
            } else {
                std::cerr << "Ninjas api problem\n";
                std::cerr << "Please check your connection\n";
                exit(0);
            }
        }

        std::ofstream city("configs/city.json");
        city << array_cities;
        city.close();

        cities_data_ = JsonProcess::ParseJson("configs/city.json");
        if (!JsonProcess::success_) {
            std::cerr << "Unable to open cities config\n";
        }
        if (RequestWeather(current_city_ind_)) {
            std::cerr << "Weather request error\nPlease check connection\n";
        }
        months_ = JsonProcess::ParseJson("configs/months.json");
        if (!JsonProcess::success_){
            std::cerr << "Unable to open months config file\n";
        }
    }
std::string Interface::GetApiKey() {
    Json api_key = JsonProcess::ParseJson("configs/api_ninjas.json");
    if (!JsonProcess::success_) {
        std::cerr << "Unable to open api key file\n";
    }
    return api_key["key"];
}
int Interface::RequestWeather(size_t ind) {
    GetWRP(ind);
    cpr::Response r_weather = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast"},
                                cpr::Parameters{{"latitude", std::to_string(wrp_.latitude)},
                                                {"longitude", std::to_string(wrp_.longitude)},
                                                {"forecast_days", std::to_string(wrp_.forecast_days)},
                                                {"hourly", "temperature_2m"},
                                                {"hourly", "weather_code"},
                                                {"hourly", "wind_speed_10m"}
                                                });
    if (r_weather.status_code != 200) {
        return 1;
    }
    Json weather_data = JsonProcess::ParseJsonString(r_weather.text);
    weather_data_ = WeatherData(weather_data);

    return 0;
}

void Interface::GetWRP(size_t ind) {
    wrp_.latitude = cities_data_[ind]["latitude"];
    wrp_.longitude = cities_data_[ind]["longitude"];
}

std::string DoubleToString(double num) {
    std::string str = std::to_string(num);

    return str.substr(0, str.find('.') + 2);
}
Component Interface::GetRenderer() const {
    return Renderer([&] () {
        std::vector<Element> day;
        std::vector<Element> days;
        for (size_t cur_day = 0; cur_day != wrp_.forecast_days; ++cur_day) {
            day.clear();
            for (int i = 0; i != 4; ++i) {
                std::string time_of_day;
                switch (i){
                    case 0:
                        time_of_day = "Night";
                        break;
                    case 1:
                        time_of_day = "Morning";
                        break;
                    case 2:
                        time_of_day = "Noon";
                        break;
                    case 3:
                        time_of_day = "Evening";
                        break;
                }
                ColorRGB color_temp;
                if (weather_data_.temps[4 * cur_day + i] >= 0) {
                    color_temp = ColorRGB(255, 69, 0);
                } else {
                    color_temp = ColorRGB(0, 0, 255);
                }
                Element tod = window(text(time_of_day) | color(Color::RGB(128,128,128)) | center,
                vbox({
                text(DoubleToString(weather_data_.temps[4 * cur_day + i]) + "°C") | color(Color::RGB(color_temp.red_, color_temp.green_ ,color_temp.blue_)),
                text(DoubleToString(weather_data_.winds[4 * cur_day + i]) + " m/s") | color(Color::RGB(128,128,128)),
                text(static_cast<std::string>(weather_types_[std::to_string(weather_data_.weather_codes[4 * cur_day + i])])) | color(Color::RGB(128,128,128))
                }) | flex);
                day.push_back(tod);
            }

            std::string date_time = weather_data_.dates[cur_day * 24];
            std::string month = months_[date_time.substr(5, 2)];
            std::string date = date_time.substr(8, 2);
            days.push_back(window(text(month + " " + date) | center | bold | color(Color::RGB(128,128,128)), hbox(day)));
        }
        
        return window(text(run_data_.cities[current_city_ind_]) | bold | color(Color::RGB(128,128,128)), vbox(days)) | color(Color::White) | bgcolor(Color::RGB(135, 206, 250));
    });
}

Component Interface::GetComponent(Component& renderer, ScreenInteractive& screen) {
    return CatchEvent(renderer, [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('n')) {
            if (!RequestWeather((current_city_ind_ + 1) % cities_count_)) {
                current_city_ind_ = (current_city_ind_ + 1) % cities_count_;
            }
            screen.PostEvent(Event::Custom);

            return true;
        }
        if (event == Event::Character('p')) {
            if (!RequestWeather((current_city_ind_ - 1) % cities_count_)) {
                current_city_ind_ = (current_city_ind_ - 1) % cities_count_;
            }
            screen.PostEvent(Event::Custom);

            return true;
        }
        if (event == Event::Character('-')) {
            wrp_.forecast_days = std::max(wrp_.forecast_days - 1, 1);
            if (RequestWeather(current_city_ind_)) {
                wrp_.forecast_days += 1;
            }
            screen.PostEvent(Event::Custom);

            return true;
        }
        if (event == Event::Character('+')) {
            wrp_.forecast_days = std::min(wrp_.forecast_days + 1, 16);
            if (RequestWeather(current_city_ind_)) {
                wrp_.forecast_days -= 1;
            }
            screen.PostEvent(Event::Custom);

            return true;
        }
        
        return false;
    });
}
Component GetHeader() {
    return Renderer([&] () {
        std::vector<std::string> header_text = JsonProcess::ParseJson("configs/art.json")["header"];
        std::vector<Element> header_lines;
        for(int i = 0; i != header_text.size(); ++i) {
            header_lines.push_back(text(header_text[i]) | color(Color::RGB(105,105,105)));
        }
        return center(vbox(header_lines)) | bgcolor(Color::RGB(135, 206, 250));
    });
}

void Interface::RunInterface() {
    auto screen = ScreenInteractive::TerminalOutput();
    
    Component renderer = GetRenderer();
    Component header = GetHeader();
    Component inter = Container::Vertical({
        header,
        renderer
    });
    Component component = GetComponent(inter, screen);

    std::thread updater([&] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(run_data_.frequency));
            RequestWeather(current_city_ind_);
            screen.Post(Event::Custom);
        }
    });
    screen.Loop(component);
    updater.detach();
}