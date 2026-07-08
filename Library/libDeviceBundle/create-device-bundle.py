#!/usr/bin/env python3
"""
Utility script to create a device bundle directory structure
"""

import os
import sys
import json
from pathlib import Path

def create_device_bundle(bundle_path, device_name, device_model, manufacturer="Unknown"):
    """Create a device bundle directory structure"""
    
    bundle_path = Path(bundle_path)
    bundle_path.mkdir(parents=True, exist_ok=True)
    
    # Create subdirectories
    (bundle_path / "drivers").mkdir(exist_ok=True)
    (bundle_path / "resources").mkdir(exist_ok=True)
    
    # Create Info.plist
    info_plist = f"""{{
    CFBundleName = "{device_name}";
    CFBundleVersion = "1.0.0";
    CFBundleIdentifier = "com.device.{device_model.lower()}";
    
    DeviceModel = "{device_model}";
    DeviceManufacturer = "{manufacturer}";
    DeviceArch = "arm64";
    DeviceTreeSource = "device.dts";
    
    CompatibleDevices = (
        "{device_model.lower()}.v1"
    );
    
    Drivers = {{}};
    
    DeviceProperties = {{}};
}}"""
    
    (bundle_path / "Info.plist").write_text(info_plist)
    
    # Create basic device tree
    dts_content = f"""
/dts-v1/;

/ {{
    model = "{device_name}";
    compatible = "{manufacturer.lower()},{device_model.lower()}";
    
    #address-cells = <2>;
    #size-cells = <2>;
}};
"""
    (bundle_path / "device.dts").write_text(dts_content)
    
    print(f"Created device bundle at: {bundle_path}")
    print(f"  - Info.plist")
    print(f"  - device.dts")
    print(f"  - drivers/")
    print(f"  - resources/")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <bundle-path> <device-name> <device-model> [manufacturer]")
        sys.exit(1)
    
    bundle_path = sys.argv[1]
    device_name = sys.argv[2]
    device_model = sys.argv[3]
    manufacturer = sys.argv[4] if len(sys.argv) > 4 else "Unknown"
    
    create_device_bundle(bundle_path, device_name, device_model, manufacturer)
