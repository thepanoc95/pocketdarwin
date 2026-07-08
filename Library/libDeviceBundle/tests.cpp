#include <iostream>
#include <cassert>
#include "DeviceInfo.hpp"
#include "DeviceBundle.hpp"

class DeviceBundleTests {
public:
    static void run() {
        std::cout << "Running libDeviceBundle tests..." << std::endl;
        
        testDeviceInfoCreation();
        testDeviceInfoProperties();
        testDeviceDrivers();
        
        std::cout << "\nAll tests passed!" << std::endl;
    }
    
private:
    static void testDeviceInfoCreation() {
        std::cout << "  [TEST] DeviceInfo Creation... ";
        
        DeviceInfo info;
        assert(!info.isValid());
        
        info.setProperty("test", "value");
        assert(info.getProperty("test") == "value");
        
        std::cout << "PASSED" << std::endl;
    }
    
    static void testDeviceInfoProperties() {
        std::cout << "  [TEST] DeviceInfo Properties... ";
        
        DeviceInfo info;
        
        // These should be set via plist in real usage
        info.setProperty("device.name", "Test Device");
        info.setProperty("device.version", "1.0.0");
        
        assert(info.getProperty("device.name") == "Test Device");
        assert(info.getProperty("device.version") == "1.0.0");
        
        std::cout << "PASSED" << std::endl;
    }
    
    static void testDeviceDrivers() {
        std::cout << "  [TEST] Device Drivers... ";
        
        DeviceInfo info;
        
        DeviceInfo::DriverInfo driver1;
        driver1.name = "TestDriver1";
        driver1.path = "drivers/test1.so";
        driver1.version = "1.0";
        
        info.addDriver(driver1);
        
        const auto* found = info.getDriver("TestDriver1");
        assert(found != nullptr);
        assert(found->name == "TestDriver1");
        assert(found->path == "drivers/test1.so");
        
        const auto& drivers = info.getDrivers();
        assert(drivers.size() == 1);
        
        std::cout << "PASSED" << std::endl;
    }
};

int main() {
    try {
        DeviceBundleTests::run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
