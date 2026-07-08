# libDeviceBundle

## Overview

libDeviceBundle is a C++ library for managing device bundles similar to NSBundle on Apple platforms. It provides functionality to parse, load, and manage device configurations, device tree sources (DTS), and driver information from structured bundle directories.

## Features

- **Bundle Management**: Load and manage device bundles from directory structures
- **Device Information**: Parse and access device metadata from Info.plist files
- **Device Tree Support**: Load and manage device tree source (DTS) files
- **Driver Management**: Enumerate and access driver information
- **Configuration Management**: Parse and manage device configuration files
- **Plist Parsing**: Built-in plist parsing via libpbxparser

## Architecture

### Core Components

#### DeviceInfo
Represents individual device information parsed from a plist file. Handles:
- Device metadata (name, model, manufacturer, version, architecture)
- Device tree source references
- Compatible device lists
- Driver information
- Custom device properties

#### DeviceBundle
Represents a complete device bundle. Provides:
- Bundle loading from filesystem
- Bundle validation
- Resource enumeration
- Configuration management
- Device tree compilation support

### File Structure

A device bundle typically has the following structure:

```
MyDevice.device/
├── Info.plist          # Device metadata
├── device.dts          # Device tree source
├── device.conf         # Configuration file
├── drivers/            # Driver binaries
│   ├── driver1.so
│   └── driver2.so
└── resources/          # Additional resources
    └── firmware.bin
```

## Building

### Using GNU Make

```bash
make
make install
```

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
make install
```

## Usage

### Loading a Device Bundle

```cpp
#include "DeviceBundle.hpp"

int main() {
    // Load a device bundle
    DeviceBundle bundle("/path/to/Device.device");
    
    if (!bundle.isValid()) {
        std::cerr << "Failed to load bundle" << std::endl;
        return 1;
    }
    
    // Access device information
    const auto& deviceInfo = bundle.getDeviceInfo();
    std::cout << "Device: " << deviceInfo.getDeviceName() << std::endl;
    std::cout << "Model: " << deviceInfo.getDeviceModel() << std::endl;
    std::cout << "Manufacturer: " << deviceInfo.getDeviceManufacturer() << std::endl;
    
    return 0;
}
```

### Accessing Drivers

```cpp
const auto& drivers = bundle.getDeviceInfo().getDrivers();
for (const auto& driver : drivers) {
    std::cout << "Driver: " << driver.name 
              << " at " << driver.path << std::endl;
}
```

### Loading Device Tree

```cpp
std::string dtsContent = bundle.loadDTS();
// Process DTS content
```

### Configuration Management

```cpp
bundle.loadConfiguration("/path/to/device.conf");
std::string value = bundle.getConfiguration("key");
```

## Info.plist Format

A typical Info.plist for a device bundle uses plist format with the following keys:

```
{
    CFBundleName = "Device Display Name";
    CFBundleVersion = "1.0";
    DeviceModel = "Model Name";
    DeviceManufacturer = "Manufacturer";
    DeviceArch = "arm64";
    DeviceTreeSource = "device.dts";
    
    CompatibleDevices = (
        "device.v1",
        "device.v2"
    );
    
    Drivers = {
        driverName = {
            Path = "drivers/driver.so";
            Version = "1.0";
            Properties = {
                prop1 = "value1";
                prop2 = "value2";
            };
        };
    };
    
    DeviceProperties = {
        property1 = "value1";
        property2 = "value2";
    };
}
```

## Dependencies

- libpbxparser (included): For plist parsing
- C++17 or later
- Standard library with filesystem support

## API Reference

### DeviceInfo

```cpp
class DeviceInfo {
    bool loadFromPlist(const std::string& plistPath);
    bool loadFromString(const std::string& plistContent);
    
    std::string getDeviceName() const;
    std::string getDeviceModel() const;
    std::string getDeviceManufacturer() const;
    std::string getDeviceVersion() const;
    std::string getDeviceArch() const;
    std::string getDTSPath() const;
    
    std::vector<std::string> getCompatibleDevices() const;
    
    const DriverInfo* getDriver(const std::string& name) const;
    const std::vector<DriverInfo>& getDrivers() const;
    void addDriver(const DriverInfo& driver);
    
    std::string getProperty(const std::string& key) const;
    void setProperty(const std::string& key, const std::string& value);
    const std::map<std::string, std::string>& getAllProperties() const;
    
    bool isValid() const;
};
```

### DeviceBundle

```cpp
class DeviceBundle {
    bool loadBundle(const std::string& bundlePath);
    void unloadBundle();
    
    std::string getBundlePath() const;
    std::string getResourcePath(const std::string& resourceName) const;
    std::string getDriverPath(const std::string& driverName) const;
    
    const DeviceInfo& getDeviceInfo() const;
    DeviceInfo& getDeviceInfo();
    
    const BundleContents& getContents() const;
    std::vector<std::string> listResources() const;
    std::vector<std::string> listDrivers() const;
    
    std::string loadDTS() const;
    bool compileDTS(const std::string& outputPath) const;
    
    bool loadConfiguration(const std::string& configFile);
    std::string getConfiguration(const std::string& key) const;
    
    bool isValid() const;
    bool hasRequiredFiles() const;
    
    std::string getBundleVersion() const;
    std::string getBundleIdentifier() const;
};
```

## License

See LICENSE file in the libDeviceBundle directory.

## Future Enhancements

- [ ] DTS compilation support using dtc
- [ ] Bundle archive loading (tar, zip)
- [ ] Hot-reload capability
- [ ] Binary plist support
- [ ] XML plist support
- [ ] Validation schema support
