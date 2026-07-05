#include <AndroidPlatform/APDeviceTree.h>
#include <AndroidPlatform/APBitfield.h>
#include <AndroidPlatform/APRuntime.h>
#include <AndroidPlatform/APGPIO.h>
#include <AndroidPlatform/APBus.h>
#include <AndroidPlatform/APClockPower.h>
#include <IOKit/IOService.h>

#pragma mark - APDeviceTree

bool ap_dt_parse(IOService* provider, struct APDeviceInfo* info)
{
    if (!provider || !info)
        return false;

    OSNumber* regBase = OSDynamicCast(OSNumber,
        provider->getProperty(AP_PROP_REG_BASE));
    OSNumber* regSize = OSDynamicCast(OSNumber,
        provider->getProperty(AP_PROP_REG_SIZE));
    OSNumber* irqNum = OSDynamicCast(OSNumber,
        provider->getProperty(AP_PROP_INTERRUPT));
    OSString* busStr = OSDynamicCast(OSString,
        provider->getProperty(AP_PROP_BUS_TYPE));
    OSNumber* busAddr = OSDynamicCast(OSNumber,
        provider->getProperty(AP_PROP_BUS_ADDR));

    info->regBasePhys   = regBase ? regBase->unsigned64BitValue() : 0;
    info->regSize       = regSize ? (ap_size_t)regSize->unsigned64BitValue() : 0;
    info->interruptIndex = irqNum  ? (int)irqNum->unsigned32BitValue() : -1;
    info->busAddress    = busAddr ? busAddr->unsigned32BitValue() : 0;
    info->deviceName    = "";
    info->compatible    = "";
    info->numClocks     = 0;

    if (busStr) {
        const char* bt = busStr->getCStringNoCopy();
        if (strcmp(bt, "i2c") == 0)      info->busType = kAPBusI2C;
        else if (strcmp(bt, "spi") == 0)  info->busType = kAPBusSPI;
        else if (strcmp(bt, "uart") == 0) info->busType = kAPBusUART;
        else                               info->busType = kAPBusMMIO;
    } else {
        info->busType = kAPBusMMIO;
    }

    OSString* nameStr = OSDynamicCast(OSString,
        provider->getProperty(AP_PROP_DEVICE_NAME));
    if (nameStr) info->deviceName = nameStr->getCStringNoCopy();

    OSString* compatStr = OSDynamicCast(OSString,
        provider->getProperty(AP_PROP_COMPATIBLE));
    if (compatStr) info->compatible = compatStr->getCStringNoCopy();

    OSArray* clkArr = OSDynamicCast(OSArray,
        provider->getProperty(AP_PROP_CLOCK));
    if (clkArr) info->numClocks = clkArr->getCount();

    return true;
}

bool ap_dt_is_compatible(IOService* provider, const char* compatible)
{
    OSString* compatStr = OSDynamicCast(OSString,
        provider->getProperty(AP_PROP_COMPATIBLE));
    if (!compatStr) return false;
    return strcmp(compatStr->getCStringNoCopy(), compatible) == 0;
}

uint32_t ap_dt_interrupt_count(IOService* provider)
{
    OSArray* irqArr = OSDynamicCast(OSArray,
        provider->getProperty(AP_PROP_INTERRUPT));
    return irqArr ? irqArr->getCount() : 0;
}

int ap_dt_interrupt_at(IOService* provider, uint32_t index)
{
    OSArray* irqArr = OSDynamicCast(OSArray,
        provider->getProperty(AP_PROP_INTERRUPT));
    if (!irqArr || index >= irqArr->getCount())
        return -1;
    OSNumber* num = OSDynamicCast(OSNumber, irqArr->getObject(index));
    return num ? (int)num->unsigned32BitValue() : -1;
}

const char* ap_dt_clock_name(IOService* provider, uint32_t index)
{
    OSArray* clkArr = OSDynamicCast(OSArray,
        provider->getProperty(AP_PROP_CLOCK));
    if (!clkArr || index >= clkArr->getCount())
        return nullptr;
    OSString* s = OSDynamicCast(OSString, clkArr->getObject(index));
    return s ? s->getCStringNoCopy() : nullptr;
}

#pragma mark - APBitfield

void ap_register_dump(volatile void* base,
                      const struct APRegField* fields,
                      uint32_t numFields,
                      const char* label)
{
    if (label) kprintf("--- %s ---\n", label);
    for (uint32_t i = 0; i < numFields; i++) {
        uint32_t val = ap_reg_read32((volatile void*)
            ((uintptr_t)base + fields[i].offset));
        if (fields[i].name) {
            kprintf("  %s (0x%03x) = 0x%08x\n",
                    fields[i].name, fields[i].offset, val);
        } else {
            kprintf("  [0x%03x] = 0x%08x\n", fields[i].offset, val);
        }
    }
}

#pragma mark - APRuntime

void ap_udelay(uint32_t usecs)
{
    uint64_t start = ap_ticks();
    uint64_t target = start + (ap_ticks_per_sec() * usecs / 1000000ULL);
    while (ap_ticks() < target) {
        __asm__ __volatile__("yield");
    }
}

void ap_mdelay(uint32_t msecs)
{
    ap_udelay(msecs * 1000);
}

#pragma mark - APGPIO

OSDefineMetaClassAndStructors(APGPIOController, IOService)

APGPIOController* APGPIOController::getController(void)
{
    OSDictionary* matching = IOService::serviceMatching("APGPIOController");
    if (!matching) return nullptr;

    IOService* matched = IOService::waitForMatchingService(matching, 5000000000ULL);
    matching->release();

    return OSDynamicCast(APGPIOController, matched);
}

#pragma mark - APBus

OSDefineMetaClassAndStructors(APBusController, IOService)

IOReturn APBusController::i2cReadReg8(uint8_t slaveAddr, uint8_t regAddr, uint8_t* val)
{
    return i2cTransfer(slaveAddr, &regAddr, 1, val, 1);
}

IOReturn APBusController::i2cWriteReg8(uint8_t slaveAddr, uint8_t regAddr, uint8_t val)
{
    uint8_t txBuf[2] = { regAddr, val };
    return i2cTransfer(slaveAddr, txBuf, 2, nullptr, 0);
}

IOReturn APBusController::i2cReadReg16(uint8_t slaveAddr, uint8_t regAddr, uint16_t* val)
{
    uint8_t rxBuf[2];
    IOReturn ret = i2cTransfer(slaveAddr, &regAddr, 1, rxBuf, 2);
    if (ret == kIOReturnSuccess)
        *val = (uint16_t)rxBuf[0] | ((uint16_t)rxBuf[1] << 8);
    return ret;
}

IOReturn APBusController::i2cWriteReg16(uint8_t slaveAddr, uint8_t regAddr, uint16_t val)
{
    uint8_t txBuf[3] = { regAddr, (uint8_t)(val & 0xFF), (uint8_t)(val >> 8) };
    return i2cTransfer(slaveAddr, txBuf, 3, nullptr, 0);
}

IOReturn APBusController::i2cReadBlock(uint8_t slaveAddr, uint8_t regAddr,
                                        uint8_t* buf, uint32_t len)
{
    return i2cTransfer(slaveAddr, &regAddr, 1, buf, len);
}

IOReturn APBusController::i2cWriteBlock(uint8_t slaveAddr, uint8_t regAddr,
                                         const uint8_t* buf, uint32_t len)
{
    uint8_t* txBuf = (uint8_t*)IOMalloc(len + 1);
    if (!txBuf) return kIOReturnNoMemory;
    txBuf[0] = regAddr;
    memcpy(txBuf + 1, buf, len);
    IOReturn ret = i2cTransfer(slaveAddr, txBuf, len + 1, nullptr, 0);
    IOFree(txBuf, len + 1);
    return ret;
}

APBusController* APBusController::getController(APBusProtocol proto, uint32_t instance)
{
    OSDictionary* matching = IOService::serviceMatching("APBusController");
    if (!matching) return nullptr;

    OSIterator* iter = IOService::getMatchingServices(matching);
    matching->release();
    if (!iter) return nullptr;

    APBusController* result = nullptr;
    OSObject* obj;
    while ((obj = iter->getNextObject()) != nullptr) {
        APBusController* ctlr = OSDynamicCast(APBusController, obj);
        if (!ctlr) continue;
        OSNumber* busAddr = OSDynamicCast(OSNumber,
            ctlr->getProperty(AP_PROP_BUS_ADDR));
        if (busAddr && busAddr->unsigned32BitValue() == instance) {
            result = ctlr;
            break;
        }
    }
    iter->release();
    return result;
}

#pragma mark - APRegisterMap

APRegisterMap::APRegisterMap(APBusController* bus, uint8_t slaveAddr,
                             uint16_t numRegs)
    : fBus(bus), fSlaveAddr(slaveAddr), fNumRegs(numRegs)
{
    fShadow = (uint8_t*)IOMalloc(numRegs);
    if (fShadow)
        memset(fShadow, 0, numRegs);
}

APRegisterMap::~APRegisterMap()
{
    if (fShadow)
        IOFree(fShadow, fNumRegs);
}

IOReturn APRegisterMap::read(uint16_t reg)
{
    if (reg >= fNumRegs || !fShadow) return kIOReturnBadArgument;
    return fBus->i2cReadReg8(fSlaveAddr, (uint8_t)reg, &fShadow[reg]);
}

IOReturn APRegisterMap::readBlock(uint16_t startReg, uint16_t count)
{
    if (startReg + count > fNumRegs || !fShadow)
        return kIOReturnBadArgument;
    return fBus->i2cReadBlock(fSlaveAddr, (uint8_t)startReg,
                               &fShadow[startReg], count);
}

IOReturn APRegisterMap::write(uint16_t reg)
{
    if (reg >= fNumRegs || !fShadow) return kIOReturnBadArgument;
    return fBus->i2cWriteReg8(fSlaveAddr, (uint8_t)reg, fShadow[reg]);
}

IOReturn APRegisterMap::writeBlock(uint16_t startReg, uint16_t count)
{
    if (startReg + count > fNumRegs || !fShadow)
        return kIOReturnBadArgument;
    return fBus->i2cWriteBlock(fSlaveAddr, (uint8_t)startReg,
                                &fShadow[startReg], count);
}

uint8_t APRegisterMap::get8(uint16_t reg)
{
    if (reg >= fNumRegs) return 0;
    return fShadow[reg];
}

uint16_t APRegisterMap::get16(uint16_t reg)
{
    if (reg + 1 >= fNumRegs) return 0;
    return (uint16_t)fShadow[reg] | ((uint16_t)fShadow[reg + 1] << 8);
}

void APRegisterMap::set8(uint16_t reg, uint8_t val)
{
    if (reg < fNumRegs) fShadow[reg] = val;
}

void APRegisterMap::set16(uint16_t reg, uint16_t val)
{
    if (reg + 1 < fNumRegs) {
        fShadow[reg]     = (uint8_t)(val & 0xFF);
        fShadow[reg + 1] = (uint8_t)(val >> 8);
    }
}

void APRegisterMap::modify(uint16_t reg, uint8_t clr, uint8_t set)
{
    if (reg < fNumRegs)
        fShadow[reg] = (fShadow[reg] & ~clr) | set;
}

#pragma mark - APClock

OSDefineMetaClassAndStructors(APClockController, IOService)

APClockController* APClockController::getController(void)
{
    OSDictionary* matching = IOService::serviceMatching("APClockController");
    if (!matching) return nullptr;

    IOService* matched = IOService::waitForMatchingService(matching, 5000000000ULL);
    matching->release();

    return OSDynamicCast(APClockController, matched);
}

#pragma mark - APPowerDomain

OSDefineMetaClassAndStructors(APPowerDomain, IOService)

APPowerDomain* APPowerDomain::getDomain(const char* name)
{
    OSDictionary* matching = IOService::serviceMatching("APPowerDomain");
    if (!matching) return nullptr;

    OSIterator* iter = IOService::getMatchingServices(matching);
    matching->release();
    if (!iter) return nullptr;

    APPowerDomain* result = nullptr;
    OSObject* obj;
    while ((obj = iter->getNextObject()) != nullptr) {
        APPowerDomain* dom = OSDynamicCast(APPowerDomain, obj);
        if (!dom) continue;
        OSString* n = OSDynamicCast(OSString,
            dom->getProperty("pmic-regulator-name"));
        if (n && strcmp(n->getCStringNoCopy(), name) == 0) {
            result = dom;
            break;
        }
    }
    iter->release();
    return result;
}
