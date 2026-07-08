#include "DeviceBundle.hpp"
#include <iostream>
#include <iomanip>

void printDeviceInfo(const DeviceBundle& bundle) {
    const auto& info = bundle.getDeviceInfo();
    
    std::cout << "=== Device Bundle Information ===" << std::endl;
    std::cout << "Bundle Path: " << bundle.getBundlePath() << std::endl;
    std::cout << "Bundle Version: " << bundle.getBundleVersion() << std::endl;
    std::cout << "Bundle ID: " << bundle.getBundleIdentifier() << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Device Information ===" << std::endl;
    std::cout << "Name: " << info.getDeviceName() << std::endl;
    std::cout << "Model: " << info.getDeviceModel() << std::endl;
    std::cout << "Manufacturer: " << info.getDeviceManufacturer() << std::endl;
    std::cout << "Version: " << info.getDeviceVersion() << std::endl;
    std::cout << "Architecture: " << info.getDeviceArch() << std::endl;
    std::cout << "DTS Path: " << info.getDTSPath() << std::endl;
    std::cout << std::endl;
    
    auto compatDevices = info.getCompatibleDevices();
    if (!compatDevices.empty()) {
        std::cout << "=== Compatible Devices ===" << std::endl;
        for (const auto& device : compatDevices) {
            std::cout << "  - " << device << std::endl;
        }
        std::cout << std::endl;
    }
    
    auto drivers = info.getDrivers();
    if (!drivers.empty()) {
        std::cout << "=== Drivers ===" << std::endl;
        for (const auto& driver : drivers) {
            std::cout << "  Driver: " << driver.name << std::endl;
            std::cout << "    Path: " << driver.path << std::endl;
            std::cout << "    Version: " << driver.version << std::endl;
            
            if (!driver.properties.empty()) {
                std::cout << "    Properties:" << std::endl;
                for (const auto& [key, value] : driver.properties) {
                    std::cout << "      " << key << " = " << value << std::endl;
                }
            }
        }
        std::cout << std::endl;
    }
    
    auto properties = info.getAllProperties();
    if (!properties.empty()) {
        std::cout << "=== Device Properties ===" << std::endl;
        for (const auto& [key, value] : properties) {
            std::cout << "  " << key << " = " << value << std::endl;
        }
        std::cout << std::endl;
    }
}

void printBundleContents(const DeviceBundle& bundle) {
    const auto& contents = bundle.getContents();
    
    std::cout << "=== Bundle Contents ===" << std::endl;
    
    if (!contents.infoPlist.empty()) {
        std::cout << "Info.plist: " << contents.infoPlist << std::endl;
    }
    
    if (!contents.dtsFile.empty()) {
        std::cout << "Device Tree Source: " << contents.dtsFile << std::endl;
    }
    
    if (!contents.configFile.empty()) {
        std::cout << "Configuration: " << contents.configFile << std::endl;
    }
    
    if (!contents.drivers.empty()) {
        std::cout << "Drivers:" << std::endl;
        for (const auto& driver : contents.drivers) {
            std::cout << "  - " << driver << std::endl;
        }
    }
    
    if (!contents.resources.empty()) {
        std::cout << "Resources:" << std::endl;
        for (const auto& resource : contents.resources) {
            std::cout << "  - " << resource << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <bundle-path>" << std::endl;
        return 1;
    }

    std::string bundlePath = argv[1];
    
    std::cout << "Loading device bundle from: " << bundlePath << std::endl;
    std::cout << std::endl;
    
    DeviceBundle bundle;
    if (!bundle.loadBundle(bundlePath)) {
        std::cerr << "Error: Failed to load device bundle" << std::endl;
        return 1;
    }
    
    if (!bundle.isValid()) {
        std::cerr << "Error: Bundle is not valid" << std::endl;
        return 1;
    }
    
    printDeviceInfo(bundle);
    printBundleContents(bundle);
    
    return 0;
}
