#pragma once

#include "common.h"
#include "Wire.h"

#define I2C0_CONFIG 0x01
#define I2C1_CONFIG 0x02
#define I2C2_CONFIG 0x04

#define I2C0_PERIPHERAL 0x10
#define I2C1_PERIPHERAL 0x20
#define I2C2_PERIPHERAL 0x40

class I2CHandler {
    public:
        I2CHandler();
        ~I2CHandler();
        TesterError i2cRead(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr);
        TesterError i2cWrite(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr);
        TesterError i2cScan(uint8_t i2c, uint8_t *address);
        TesterError setConfiguration(uint8_t i2c, uint8_t address, uint32_t frequency, bool controller);
        TesterError unsetConfiguration(uint8_t i2c);
        bool isConfigured(uint8_t i2c);
    private:

        TesterError handleControllerRead(TwoWire *i2c, uint8_t *data, size_t size);
        TesterError handlePeripheralRead(uint8_t i2c, uint8_t *data);
};