#pragma once

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

typedef struct i2c_handler_data_t
{
    TwoWire *bus;
    uint8_t data_register[REGISTER_COUNT];
    uint8_t peripheral_address;
    bool is_configured;
    bool is_controller;
    uint32_t controller_frequency;
};

class I2CHandler : public CmdHandler {
    private:
        static i2c_handler_data_t i2c_data[I2C_COUNT];

        virtual chAT::CommandStatus handle_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_write(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_test(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_read(chAT::Server&, chAT::ATParser&);
        virtual chAT::CommandStatus handle_cfg_write(chAT::Server&, chAT::ATParser&);

        int i2c_read(uint8_t bus_number, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr);
        int i2c_write(uint8_t bus_number, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr);
        int i2c_scan(uint8_t bus_number, uint8_t *address);
        int set_configuration(uint8_t bus_number, uint8_t address, uint32_t frequency, bool controller);
        int unset_configuration(uint8_t bus_number);

        int handle_controller_read(TwoWire *i2c, uint8_t *data, size_t size);
        int handle_peripheral_read(uint8_t bus_number, uint8_t *data);


        //CALLBACKS
        static void on_receive_callback(uint8_t bus_number, int numBytes);

        static void on_receive_callback_0(int numBytes);
        static void on_receive_callback_1(int numBytes);
        static void on_receive_callback_2(int numBytes);

        static void on_request_callback(uint8_t bus_number);

        static void on_request_callback_0();
        static void on_request_callback_1();
        static void on_request_callback_2();
        //END OF CALLBACKS
    public:
        I2CHandler() : CmdHandler("+I2C") {}
        virtual ~I2CHandler() {}
};