#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "libpbxparser/plist_decoder.hpp"

/**
 * @brief Device information parsed from Info.plist in a device bundle
 */
class DeviceInfo {
public:
    struct DeviceProperty {
        std::string name;
        std::string value;
    };

    struct DriverInfo {
        std::string name;
        std::string path;
        std::string version;
        std::map<std::string, std::string> properties;
    };

public:
    DeviceInfo();
    explicit DeviceInfo(const std::string& plistPath);
    ~DeviceInfo() = default;

    // Plist loading and parsing
    bool loadFromPlist(const std::string& plistPath);
    bool loadFromString(const std::string& plistContent);

    // Getters for device information
    std::string getDeviceName() const;
    std::string getDeviceModel() const;
    std::string getDeviceManufacturer() const;
    std::string getDeviceVersion() const;
    std::string getDeviceArch() const;
    
    // Device tree source (DTS) related
    std::string getDTSPath() const;
    std::vector<std::string> getCompatibleDevices() const;

    // Driver management
    void addDriver(const DriverInfo& driver);
    const std::vector<DriverInfo>& getDrivers() const;
    const DriverInfo* getDriver(const std::string& name) const;

    // Device properties
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    const std::map<std::string, std::string>& getAllProperties() const;

    // Validation
    bool isValid() const;

private:
    void _parsePlistDictionary(const pbx::Dictionary& dict);
    void _extractDrivers(const pbx::Dictionary& dict);
    void _extractProperties(const pbx::Dictionary& dict);

    std::string deviceName_;
    std::string deviceModel_;
    std::string deviceManufacturer_;
    std::string deviceVersion_;
    std::string deviceArch_;
    std::string dtsPath_;
    
    std::vector<std::string> compatibleDevices_;
    std::vector<DriverInfo> drivers_;
    std::map<std::string, std::string> properties_;
    
    bool valid_;
};
