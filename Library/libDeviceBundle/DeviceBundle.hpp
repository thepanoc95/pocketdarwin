#pragma once

#include "DeviceInfo.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

/**
 * @brief Represents a device bundle (similar to NSBundle on Apple platforms)
 * Contains device configurations, device tree sources, drivers, and metadata
 */
class DeviceBundle {
public:
    struct BundleContents {
        std::string infoPlist;      // Info.plist path
        std::string dtsFile;        // Device tree source file
        std::string configFile;     // Device configuration file
        std::vector<std::string> drivers;  // Driver files
        std::vector<std::string> resources; // Other resource files
    };

public:
    DeviceBundle() = default;
    explicit DeviceBundle(const std::string& bundlePath);
    ~DeviceBundle() = default;

    // Bundle lifecycle
    bool loadBundle(const std::string& bundlePath);
    bool loadBundleFromArchive(const std::string& archivePath);
    void unloadBundle();

    // Path management
    std::string getBundlePath() const;
    std::string getResourcePath(const std::string& resourceName) const;
    std::string getDriverPath(const std::string& driverName) const;

    // Device information access
    const DeviceInfo& getDeviceInfo() const;
    DeviceInfo& getDeviceInfo();

    // Bundle contents enumeration
    const BundleContents& getContents() const;
    std::vector<std::string> listResources() const;
    std::vector<std::string> listDrivers() const;

    // DTS (Device Tree Source) handling
    std::string loadDTS() const;
    bool compileDTS(const std::string& outputPath) const;

    // Configuration handling
    bool loadConfiguration(const std::string& configFile);
    std::string getConfiguration(const std::string& key) const;

    // Validation
    bool isValid() const;
    bool hasRequiredFiles() const;

    // Bundle inspection
    std::string getBundleVersion() const;
    std::string getBundleIdentifier() const;

private:
    bool _scanBundleContents();
    bool _loadInfoPlist();
    bool _validateBundle();
    
    std::string bundlePath_;
    BundleContents contents_;
    std::unique_ptr<DeviceInfo> deviceInfo_;
    std::map<std::string, std::string> configuration_;
    bool valid_;
};
