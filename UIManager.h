#pragma once
#include <fstream>
#include <sstream> // Include for std::stringstream
#include <string>
#include <unordered_map>
#include <vector> // Include if not already included

class UIConfigManager {
private:
    std::string configFilePath;
    std::unordered_map<std::string, std::string> configMap;

public:
    UIConfigManager(const std::string& filePath) : configFilePath(filePath) {}

    void loadConfig() {
        std::ifstream configFile(configFilePath);
        if (configFile) {
            std::string line;
            while (std::getline(configFile, line)) {
                auto delimiterPos = line.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = line.substr(0, delimiterPos);
                    std::string value = line.substr(delimiterPos + 1);
                    configMap[key] = value;
                }
            }
        }
    }

    void saveConfig() {
        std::ofstream configFile(configFilePath);
        for (const auto& pair : configMap) {
            configFile << pair.first << "=" << pair.second << std::endl;
        }
    }

    std::string getValue(const std::string& key) {
        auto it = configMap.find(key);
        if (it != configMap.end()) {
            return it->second;
        }
        return "";
    }

    void setValue(const std::string& key, const std::string& value) {
        configMap[key] = value;
    }

    void saveComponentConfig(const std::string& key, const std::vector<std::string>& componentProperties) {
        std::string serialized = "";
        for (const auto& prop : componentProperties) {
            serialized += prop + ";";
        }
        setValue(key, serialized);
    }

    std::vector<std::string> loadComponentConfig(const std::string& key) {
        std::vector<std::string> properties;
        std::string serialized = getValue(key);
        std::stringstream ss(serialized);
        std::string item;
        while (std::getline(ss, item, ';')) {
            if (!item.empty()) { // Check to avoid adding empty strings
                properties.push_back(item);
            }
        }
        return properties;
    }
};
