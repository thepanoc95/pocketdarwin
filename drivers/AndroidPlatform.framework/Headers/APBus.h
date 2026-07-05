#ifndef _AP_BUS_H_
#define _AP_BUS_H_

#include <AndroidPlatform/APTypes.h>
#include <IOKit/IOService.h>

/* ------------------------------------------------------------------ *
 * APBus — abstract I2C / SPI bus interface
 *
 * Peripheral drivers (touch, sensors, audio codec, etc.) use this API
 * instead of raw register access.  The bus controller driver publishes
 * an APBusController service in the IORegistry; peripheral drivers
 * find it by matching on the "android,bus-type" and "android,bus-addr"
 * properties.
 * ------------------------------------------------------------------ */

enum APBusProtocol {
    kAPBusProtocolI2C = 1,
    kAPBusProtocolSPI = 2,
};

struct APBusMessage {
    uint8_t*  txData;
    uint32_t  txLen;
    uint8_t*  rxData;
    uint32_t  rxLen;
    bool      stop;
    uint32_t  timeoutMs;
};

class APBusController : public IOService
{
    OSDeclareDefaultStructors(APBusController)

public:
    /* I2C: combined write-then-read transfer to a slave address. */
    virtual IOReturn i2cTransfer(uint8_t slaveAddr,
                                 uint8_t* txBuf, uint32_t txLen,
                                 uint8_t* rxBuf, uint32_t rxLen) = 0;

    /* SPI: full-duplex transfer to a chip select. */
    virtual IOReturn spiTransfer(uint8_t cs,
                                 uint8_t* txBuf, uint8_t* rxBuf,
                                 uint32_t len, uint8_t bitsPerWord) = 0;

    /* Register-based helpers (common for I2C sensor/PMIC devices). */
    IOReturn i2cReadReg8(uint8_t slaveAddr, uint8_t regAddr, uint8_t* val);
    IOReturn i2cWriteReg8(uint8_t slaveAddr, uint8_t regAddr, uint8_t val);
    IOReturn i2cReadReg16(uint8_t slaveAddr, uint8_t regAddr, uint16_t* val);
    IOReturn i2cWriteReg16(uint8_t slaveAddr, uint8_t regAddr, uint16_t val);
    IOReturn i2cReadBlock(uint8_t slaveAddr, uint8_t regAddr,
                          uint8_t* buf, uint32_t len);
    IOReturn i2cWriteBlock(uint8_t slaveAddr, uint8_t regAddr,
                           const uint8_t* buf, uint32_t len);

    /* Utility: find the bus controller for a given type. */
    static APBusController* getController(APBusProtocol proto, uint32_t instance);
};

/* ------------------------------------------------------------------ *
 * APRegisterMap — cached shadow register set for bus-attached devices
 *
 * Useful for devices that talk over I2C/SPI but have many registers
 * (e.g. PMICs, audio codecs, touch controllers).  The shadow copy
 * avoids bus traffic on read-modify-write sequences.
 * ------------------------------------------------------------------ */

class APRegisterMap {
public:
    APRegisterMap(APBusController* bus, uint8_t slaveAddr,
                  uint16_t numRegs);
    ~APRegisterMap();

    IOReturn read(uint16_t reg);
    IOReturn readBlock(uint16_t startReg, uint16_t count);
    IOReturn write(uint16_t reg);
    IOReturn writeBlock(uint16_t startReg, uint16_t count);

    uint8_t  get8(uint16_t reg);
    uint16_t get16(uint16_t reg);
    void     set8(uint16_t reg, uint8_t val);
    void     set16(uint16_t reg, uint16_t val);
    void     modify(uint16_t reg, uint8_t clr, uint8_t set);

private:
    APBusController* fBus;
    uint8_t   fSlaveAddr;
    uint16_t  fNumRegs;
    uint8_t*  fShadow;
};

#endif /* _AP_BUS_H_ */
