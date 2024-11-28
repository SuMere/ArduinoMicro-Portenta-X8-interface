#pragma once

#include "common.h"

#include "Wire.h"
#include "cmd_handler.h"

#define I2C0_CONFIG 0x01
#define I2C1_CONFIG 0x02
#define I2C2_CONFIG 0x04

#define I2C0_PERIPHERAL 0x10
#define I2C1_PERIPHERAL 0x20
#define I2C2_PERIPHERAL 0x40

#define REGISTER_COUNT 2

#define I2C_COUNT 3

class I2CHandler : public CmdHandler {
    private:
        bool is_confgured[I2C_COUNT] = {false};
        bool is_controller[I2C_COUNT] = {false};
        TwoWire *i2c_array[I2C_COUNT] = {&Wire, &Wire1, &Wire2};

        static uint8_t data_register[I2C_COUNT][REGISTER_COUNT];
        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&);

        TesterError i2c_read(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr);
        TesterError i2c_write(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr);
        TesterError i2c_scan(uint8_t i2c, uint8_t *address);
        TesterError set_configuration(uint8_t i2c, uint8_t address, uint32_t frequency, bool controller);
        TesterError unset_configuration(uint8_t i2c);

        TesterError handle_controller_read(TwoWire *i2c, uint8_t *data, size_t size);
        TesterError handle_peripheral_read(uint8_t i2c, uint8_t *data);

        bool is_configured(uint8_t i2c);

        //CALLBACKS
        static void on_receive_callback_0(int numBytes);
        static void on_receive_callback_1(int numBytes);
        static void on_receive_callback_2(int numBytes);

        static void on_request_callback_0();
        static void on_request_callback_1();
        static void on_request_callback_2();
        //END OF CALLBACKS
    public:
        I2CHandler(CAtHandler *parent);
        virtual ~I2CHandler() {}
};