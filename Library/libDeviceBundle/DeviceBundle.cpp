#include "DeviceBundle.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

DeviceBundle::DeviceBundle(const std::string& bundlePath)
    : bundlePath_(bundlePath), valid_(false) {
    loadBundle(bundlePath);
}

bool DeviceBundle::loadBundle(const std::string& bundlePath) {
    bundlePath_ = bundlePath;
    
    if (!fs::exists(bundlePath_)) {
        std::cerr << "Bundle path does not exist: " << bundlePath_ << std::endl;
        return false;
    }

    if (!fs::is_directory(bundlePath_)) {
        std::cerr << "Bundle path is not a directory: " << bundlePath_ << std::endl;
        return false;
    }

    if (!_scanBundleContents()) {
        std::cerr << "Failed to scan bundle contents" << std::endl;
        return false;
    }

    if (!_loadInfoPlist()) {
        std::cerr << "Failed to load Info.plist" << std::endl;
        return false;
    }

    if (!_validateBundle()) {
        std::cerr << "Bundle validation failed" << std::endl;
        return false;
    }

    valid_ = true;
    return true;
}

bool DeviceBundle::loadBundleFromArchive(const std::string& archivePath) {
    std::cerr << "Archive loading not yet implemented" << std::endl;
    return false;
}

void DeviceBundle::unloadBundle() {
    bundlePath_.clear();
    contents_.infoPlist.clear();
    contents_.dtsFile.clear();
    contents_.configFile.clear();
    contents_.drivers.clear();
    contents_.resources.clear();
    deviceInfo_.reset();
    configuration_.clear();
    valid_ = false;
}

bool DeviceBundle::_scanBundleContents() {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(bundlePath_)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            std::string filename = entry.path().filename().string();
            std::string filepath = entry.path().string();
            std::string relativePath = fs::relative(entry.path(), bundlePath_).string();

            auto ends_with = [](const std::string& str, const std::string& suffix) {
                return str.size() >= suffix.size() && 
                       str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
            };

            if (filename == "Info.plist") {
                contents_.infoPlist = filepath;
            } else if (ends_with(filename, ".dts") || ends_with(filename, ".dtsi")) {
                contents_.dtsFile = filepath;
            } else if (filename == "device.conf" || filename == "config.plist") {
                contents_.configFile = filepath;
            } else if (ends_with(filename, ".so") || ends_with(filename, ".o") || 
                       ends_with(filename, ".dylib") || ends_with(filename, ".dll")) {
                contents_.drivers.push_back(filepath);
            } else {
                contents_.resources.push_back(filepath);
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error scanning bundle: " << e.what() << std::endl;
        return false;
    }
}

bool DeviceBundle::_loadInfoPlist() {
    if (contents_.infoPlist.empty()) {
        std::cerr << "No Info.plist found in bundle" << std::endl;
        return false;
    }

    deviceInfo_ = std::make_unique<DeviceInfo>(contents_.infoPlist);
    return deviceInfo_->isValid();
}

bool DeviceBundle::_validateBundle() {
    if (!deviceInfo_ || !deviceInfo_->isValid()) {
        std::cerr << "Device info is invalid" << std::endl;
        return false;
    }

    if (contents_.dtsFile.empty()) {
        std::cerr << "Warning: No device tree source found in bundle" << std::endl;
    }

    return true;
}

std::string DeviceBundle::getBundlePath() const {
    return bundlePath_;
}

std::string DeviceBundle::getResourcePath(const std::string& resourceName) const {
    for (const auto& resource : contents_.resources) {
        if (resource.find(resourceName) != std::string::npos) {
            return resource;
        }
    }
    return "";
}

std::string DeviceBundle::getDriverPath(const std::string& driverName) const {
    for (const auto& driver : contents_.drivers) {
        if (driver.find(driverName) != std::string::npos) {
            return driver;
        }
    }
    return "";
}

const DeviceInfo& DeviceBundle::getDeviceInfo() const {
    if (!deviceInfo_) {
        throw std::runtime_error("Device info not loaded");
    }
    return *deviceInfo_;
}

DeviceInfo& DeviceBundle::getDeviceInfo() {
    if (!deviceInfo_) {
        throw std::runtime_error("Device info not loaded");
    }
    return *deviceInfo_;
}

const DeviceBundle::BundleContents& DeviceBundle::getContents() const {
    return contents_;
}

std::vector<std::string> DeviceBundle::listResources() const {
    return contents_.resources;
}

std::vector<std::string> DeviceBundle::listDrivers() const {
    return contents_.drivers;
}

std::string DeviceBundle::loadDTS() const {
    if (contents_.dtsFile.empty()) {
        return "";
    }

    std::ifstream dtsFile(contents_.dtsFile);
    if (!dtsFile.is_open()) {
        std::cerr << "Failed to open DTS file: " << contents_.dtsFile << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << dtsFile.rdbuf();
    return buffer.str();
}

bool DeviceBundle::compileDTS(const std::string& outputPath) const {
    std::cerr << "DTS compilation not yet implemented" << std::endl;
    return false;
}

bool DeviceBundle::loadConfiguration(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << configFile << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }

        size_t delimPos = line.find('=');
        if (delimPos != std::string::npos) {
            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            configuration_[key] = value;
        }
    }

    return true;
}

std::string DeviceBundle::getConfiguration(const std::string& key) const {
    auto it = configuration_.find(key);
    if (it != configuration_.end()) {
        return it->second;
    }
    return "";
}

bool DeviceBundle::isValid() const {
    return valid_;
}

bool DeviceBundle::hasRequiredFiles() const {
    return !contents_.infoPlist.empty() && 
           !contents_.dtsFile.empty();
}

std::string DeviceBundle::getBundleVersion() const {
    if (!deviceInfo_) {
        return "";
    }
    return deviceInfo_->getDeviceVersion();
}

std::string DeviceBundle::getBundleIdentifier() const {
    if (!deviceInfo_) {
        return "";
    }
    return deviceInfo_->getDeviceName();
}
