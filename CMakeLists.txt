cmake_minimum_required(VERSION 3.12)

project(
    ConsoleWeather
    VERSION 1.0
    DESCRIPTION "Weather Forecast Console Application"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)


add_subdirectory(lib)
add_subdirectory(bin)