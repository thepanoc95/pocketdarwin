#include "DeviceInfo.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

DeviceInfo::DeviceInfo() 
    : valid_(false) {
}

DeviceInfo::DeviceInfo(const std::string& plistPath) 
    : valid_(false) {
    loadFromPlist(plistPath);
}

bool DeviceInfo::loadFromPlist(const std::string& plistPath) {
    std::ifstream plistFile(plistPath);
    if (!plistFile.is_open()) {
        std::cerr << "Failed to open plist file: " << plistPath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << plistFile.rdbuf();
    plistFile.close();

    return loadFromString(buffer.str());
}

bool DeviceInfo::loadFromString(const std::string& plistContent) {
    try {
        pbx::Dictionary plist = pbx::PlistDecoder::parse(plistContent);
        _parsePlistDictionary(plist);
        valid_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse plist: " << e.what() << std::endl;
        valid_ = false;
        return false;
    }
}

void DeviceInfo::_parsePlistDictionary(const pbx::Dictionary& dict) {
    // Extract basic device information
    auto nameIt = dict.find("CFBundleName");
    if (nameIt != dict.end()) {
        deviceName_ = nameIt->second.string_value();
    }

    auto modelIt = dict.find("DeviceModel");
    if (modelIt != dict.end()) {
        deviceModel_ = modelIt->second.string_value();
    }

    auto manufacturerIt = dict.find("DeviceManufacturer");
    if (manufacturerIt != dict.end()) {
        deviceManufacturer_ = manufacturerIt->second.string_value();
    }

    auto versionIt = dict.find("CFBundleVersion");
    if (versionIt != dict.end()) {
        deviceVersion_ = versionIt->second.string_value();
    }

    auto archIt = dict.find("DeviceArch");
    if (archIt != dict.end()) {
        deviceArch_ = archIt->second.string_value();
    }

    auto dtsIt = dict.find("DeviceTreeSource");
    if (dtsIt != dict.end()) {
        dtsPath_ = dtsIt->second.string_value();
    }

    auto compatIt = dict.find("CompatibleDevices");
    if (compatIt != dict.end() && compatIt->second.type() == pbx::Value::ArrayType) {
        const auto& array = compatIt->second.array_value();
        for (const auto& item : array) {
            compatibleDevices_.push_back(item.string_value());
        }
    }

    _extractDrivers(dict);
    _extractProperties(dict);
}

void DeviceInfo::_extractDrivers(const pbx::Dictionary& dict) {
    auto driversIt = dict.find("Drivers");
    if (driversIt != dict.end() && driversIt->second.type() == pbx::Value::DictionaryType) {
        try {
            const auto& driversDict = driversIt->second.dictionary_value();
            for (const auto& driverEntry : driversDict) {
                DriverInfo driver;
                driver.name = driverEntry.first;
                
                if (driverEntry.second.type() == pbx::Value::DictionaryType) {
                    const auto& driverDict = driverEntry.second.dictionary_value();
                    
                    auto pathIt = driverDict.find("Path");
                    if (pathIt != driverDict.end()) {
                        driver.path = pathIt->second.string_value();
                    }
                    
                    auto versionIt = driverDict.find("Version");
                    if (versionIt != driverDict.end()) {
                        driver.version = versionIt->second.string_value();
                    }
                    
                    auto propsIt = driverDict.find("Properties");
                    if (propsIt != driverDict.end() && 
                        propsIt->second.type() == pbx::Value::DictionaryType) {
                        const auto& propsDict = propsIt->second.dictionary_value();
                        for (const auto& prop : propsDict) {
                            driver.properties[prop.first] = prop.second.string_value();
                        }
                    }
                }
                
                drivers_.push_back(driver);
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to extract drivers: " << e.what() << std::endl;
        }
    }
}

void DeviceInfo::_extractProperties(const pbx::Dictionary& dict) {
    auto propsIt = dict.find("DeviceProperties");
    if (propsIt != dict.end() && propsIt->second.type() == pbx::Value::DictionaryType) {
        try {
            const auto& propsDict = propsIt->second.dictionary_value();
            for (const auto& prop : propsDict) {
                properties_[prop.first] = prop.second.string_value();
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to extract properties: " << e.what() << std::endl;
        }
    }
}

std::string DeviceInfo::getDeviceName() const {
    return deviceName_;
}

std::string DeviceInfo::getDeviceModel() const {
    return deviceModel_;
}

std::string DeviceInfo::getDeviceManufacturer() const {
    return deviceManufacturer_;
}

std::string DeviceInfo::getDeviceVersion() const {
    return deviceVersion_;
}

std::string DeviceInfo::getDeviceArch() const {
    return deviceArch_;
}

std::string DeviceInfo::getDTSPath() const {
    return dtsPath_;
}

std::vector<std::string> DeviceInfo::getCompatibleDevices() const {
    return compatibleDevices_;
}

void DeviceInfo::addDriver(const DriverInfo& driver) {
    drivers_.push_back(driver);
}

const std::vector<DeviceInfo::DriverInfo>& DeviceInfo::getDrivers() const {
    return drivers_;
}

const DeviceInfo::DriverInfo* DeviceInfo::getDriver(const std::string& name) const {
    for (const auto& driver : drivers_) {
        if (driver.name == name) {
            return &driver;
        }
    }
    return nullptr;
}

void DeviceInfo::setProperty(const std::string& key, const std::string& value) {
    properties_[key] = value;
}

std::string DeviceInfo::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    return "";
}

const std::map<std::string, std::string>& DeviceInfo::getAllProperties() const {
    return properties_;
}

bool DeviceInfo::isValid() const {
    return valid_ && !deviceName_.empty();
}
