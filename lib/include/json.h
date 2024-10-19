#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

#include "nlohmann/json.hpp"


using Json = nlohmann::json;

class JsonProcess {
public:
    JsonProcess() = default;
    ~JsonProcess() = default;
    static Json ParseJson(const std::filesystem::path& path_to_json);
    static Json ParseJsonString(const std::string& json_string);
    static void WriteJson(const std::filesystem::path& path_to_json, const std::string& data);
    inline static bool success_;
};