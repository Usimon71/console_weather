#include "json.h"   

Json JsonProcess::ParseJson(const std::filesystem::path& path_to_json) {
    std::ifstream file(path_to_json);
    if (!file.is_open()) {
        std::cerr << "Unable to open config file " << path_to_json << '\n';
        success_ = false;

        return Json{};
    }
    Json data = Json::parse(file);
    success_ = true;
    file.close();

    return data;
}

Json JsonProcess::ParseJsonString(const std::string& json_string) {
    return Json::parse(json_string);
}

void JsonProcess::WriteJson(const std::filesystem::path& path_to_json, const std::string& data) {
    std::ofstream file(path_to_json);
    if (!file.is_open()) {
        std::cerr << "Unable to open config file " << path_to_json << '\n';
        success_ = false;
    }
    file << data;
    success_ = true;
    file.close();
}