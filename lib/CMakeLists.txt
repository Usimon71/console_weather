set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG bb01c8d)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v4.0.0
)

FetchContent_MakeAvailable(json cpr ftxui)

add_library(weather include/json.h src/json.cpp 
                    include/weather.h src/weather.cpp
                    include/interface.h src/interface.cpp)
target_include_directories(weather PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(weather PUBLIC cpr::cpr
                                      nlohmann_json::nlohmann_json
                                      ftxui::screen
                                      ftxui::dom
                                      ftxui::component)