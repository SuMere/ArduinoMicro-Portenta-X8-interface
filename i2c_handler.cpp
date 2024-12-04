#include "i2c_handler.h"

#include "at_handler.h"

uint8_t I2CHandler::data_register[I2C_COUNT][REGISTER_COUNT] = {{0}};

chAT::CommandStatus I2CHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser)
{
    uint8_t *data = NULL;
    size_t data_size = 1;

    if (parser.args.size() != 1 &&
        parser.args.size() != 2 &&
        parser.args.size() != 4)
    {
        return write_error_message(srv, "Invalid number of arguments");
    }

    uint8_t bus_number = atoi(parser.args[0].c_str());
    if(bus_number < 0 || bus_number > I2C_COUNT){
        return write_error_message(srv, "Invalid I2C bus number");
    }

    switch (parser.args.size())
    {
    case 1: // GET REGISTER STATUS AS PERIPHERAL
    {
        if(is_controller[bus_number]){
            return write_error_message(srv, "Selected bus number should be configured as peripheral");
        }
        if(i2c_read(bus_number, 0, 0, data, REGISTER_COUNT, false) != NO_ERROR){
            return write_error_message(srv, "Error reading I2C");
        }
        String message;
        for (int i = 0; i < data_size; i++){
            message += String(data[i], HEX)+ " ";
        }
        free(data);
        return write_ok_message(srv, message.c_str());
    }
    case 2: // SCAN
    {
        if(parser.args[1] != "SCAN"){
            return write_error_message(srv, "Invalid argument");
        }
        uint8_t scan_res[119] = {0};
        if(i2c_scan(bus_number, scan_res) != 0){
            return write_error_message(srv, "Error scanning I2C");
        }
        String message = "\n    ";
        for (int i = 0; i < 16; i++) {
            message += String(i, HEX) + "  ";
        }
        message += "\n";
        for (int i=0; i < 8; i++) {
            if (i == 0) {
                message += " 0: ";
            } else {
                message += String(i * 16, HEX) + ": ";
            }
            for (int j=0; j < 16; j++) {
                if (i * 16 + j < 0x04 || i * 16 + j > 0x77) {
                    message += "   ";
                    continue;
                }
                if (scan_res[i * 16 + j] == 1) {
                    message += String(i * 16 + j, HEX) + " ";
                } else {
                    message += "-- ";
                }
            }
            message += "\n";
        }
        return write_ok_message(srv, message.c_str());
    }
    case 4: // READ AS CONTROLLER
    {
        uint8_t address = (uint8_t)strtol(parser.args[1].c_str(), NULL, 16);
        uint8_t reg_addr = 0;
        bool has_reg_addr = (parser.args[2] != "");
        if (address < 0 || address > 127){
            return write_error_message(srv, "Invalid I2C address");
        }
        if(has_reg_addr){
            reg_addr = (uint8_t)strtol(parser.args[2].c_str(), NULL, 16);
        }
        data_size = parser.args[3] != "" ? atoi(parser.args[3].c_str()) : 1;
        data = (uint8_t *)malloc(data_size);
        if(is_controller[bus_number] == false){
            return write_error_message(srv, "Selected bus number should be configured as controller");
        }
        if(i2c_read(bus_number, address, reg_addr, data, data_size, has_reg_addr) != 0){
            return write_error_message(srv, "Error reading I2C");
        }
        String message ="I2C read result from device "+String(address, HEX);
        if(has_reg_addr){
            message += ", register: "+String(reg_addr, HEX);
        }
        message += ", data: ";
        for (int i = 0; i < data_size; i++){
            message += String(data[i], HEX)+ " ";
        }
        return write_ok_message(srv, message.c_str());
    }
    }
}

chAT::CommandStatus I2CHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 4) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    uint8_t bus_number = atoi(parser.args[0].c_str());
    if(bus_number < 0 || bus_number > I2C_COUNT){
        return write_error_message(srv, "Invalid I2C bus number");
    }
    if(is_controller[bus_number] == false){
        return write_error_message(srv, "Selected bus number should be configured as controller");
    }

    uint8_t address = (uint8_t)strtol(parser.args[1].c_str(), NULL, 16);
    if (address < 0 || address > 127){
        return write_error_message(srv, "Invalid I2C address");
        srv.write_response_prompt();
    }

    uint8_t data = (uint8_t)strtol(parser.args[2].c_str(), NULL, 16);
    size_t data_size = parser.args[2].length();
    uint8_t reg_addr = 0;
    bool has_reg_addr = (parser.args[3] != "");
    if(has_reg_addr){
        reg_addr = (uint8_t)strtol(parser.args[3].c_str(), NULL, 16);
    }

    if(i2c_write(bus_number, address, reg_addr, &data, data_size, has_reg_addr) != 0){
        return write_error_message(srv, "Error writing I2C");
    }

    return write_ok_message(srv, "");
}

chAT::CommandStatus I2CHandler::handle_cfg_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    uint8_t bus_number = atoi(parser.args[0].c_str());
    if(bus_number < 0 || bus_number > I2C_COUNT){
        return write_error_message(srv, "Invalid I2C bus number");
    }

    if(is_confgured[bus_number] == false){
        return write_error_message(srv, ("I2C bus "+String(bus_number)+" not configured").c_str());
    }

    String message = "I2C bus "+String(bus_number)+" configured as ";
    if(is_controller[bus_number]){
        message += "controller";
    }else{
        message += "peripheral";
    }

    return write_ok_message(srv, message.c_str());
}

chAT::CommandStatus I2CHandler::handle_cfg_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1 && parser.args.size() != 3) {
        return write_error_message(srv, "Invalid number of arguments");
    }

    uint8_t bus_number = atoi(parser.args[0].c_str());
    if(bus_number < 0 || bus_number > I2C_COUNT){
        return write_error_message(srv, "Invalid I2C bus number");
    }

    if(parser.args.size() == 1) //CLEAR CONFIGURATION
    {
        if(unset_configuration(bus_number) != 0){
            return write_error_message(srv, "Error clearing I2C bus configuration");
        }
        return write_ok_message(srv, ("I2C bus "+String(bus_number)+" configuration cleared").c_str());
    }

    uint8_t address = 0;
    uint32_t bus_speed = atoi(parser.args[1].c_str());
    bool is_controller = parser.args[2].c_str() == "" ? false : true;
    if(is_controller == false){
        address = (uint8_t)strtol(parser.args[2].c_str(), NULL, 16);
        if (address < 0 || address > 127){
            return write_error_message(srv, "Invalid I2C address");
        }
    }

    if(set_configuration(bus_number, address, bus_speed*1000, is_controller) != 0){
        return write_error_message(srv, "Error configuring I2C bus");
    }
    return write_ok_message(srv, ("I2C bus "+String(bus_number)+" configured").c_str());

}

int I2CHandler::i2c_read(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr) {
    TwoWire *curr_i2c = NULL;

    if(i2c >= I2C_COUNT) {
        return EINVAL;
    }

    if(is_confgured[i2c] == false) {
        return EPERM;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ENODEV;
    }

    if(is_controller[i2c] == false){
        return EPERM;
    }

    curr_i2c->beginTransmission(address);
    if(register_address != 0) {
        curr_i2c->requestFrom(address, size);
        if (has_reg_addr) {
            curr_i2c->write(register_address);
        } 
    }
    if(curr_i2c->endTransmission(true) != 0) {
        return EIO;
    }

    return handle_controller_read(curr_i2c, data, size);
}

int I2CHandler::i2c_write(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr) {
    TwoWire *curr_i2c = NULL;
    
    if (i2c >= I2C_COUNT)
    {
        return EINVAL;
    }

    if (is_confgured[i2c] == false || is_controller[i2c] == false)
    {
        return EPERM;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ENODEV;
    }

    curr_i2c->beginTransmission(address);
    if (has_reg_addr) {
        curr_i2c->write(register_address);
    }

    for(size_t i = 0; i < size; i++) {
        curr_i2c->write(data[i]);
    }
    if(curr_i2c->endTransmission() != 0) {
        return EIO;
    }

    return 0;
}

int I2CHandler::i2c_scan(uint8_t i2c, uint8_t *addresses) {
    TwoWire *curr_i2c = NULL;

    if (i2c >= I2C_COUNT)
    {
        return EINVAL;
    }

    if(is_confgured[i2c] == false || is_controller[i2c] == false) {
        return EPERM;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ENODEV;
    }

    for (uint8_t i = 3; i < 119; i++)
    {
        curr_i2c->beginTransmission(i);
        if (curr_i2c->endTransmission() == 0)
        {
            addresses[i] = 1;
        }
    }

    return 0;
}

int I2CHandler::set_configuration(uint8_t i2c, uint8_t address, uint32_t frequency, bool is_controller) {
    TwoWire *curr_i2c = NULL;

    if(i2c >= I2C_COUNT) {
        return EINVAL;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ENODEV;
    }

    if(is_controller){
        curr_i2c->begin();
        this->is_controller[i2c] = true;
    }else{
        curr_i2c->begin(address);
        Serial.println("I2C address: "+String(address, HEX));
        this->is_controller[i2c] = false;
        switch (i2c)
        {
        case 0:
            curr_i2c->onRequest(on_request_callback_0);
            curr_i2c->onReceive(on_receive_callback_0);
            break;
        case 1:
            curr_i2c->onRequest(on_request_callback_1);
            curr_i2c->onReceive(on_receive_callback_1);
            break;
        case 2:
            curr_i2c->onRequest(on_request_callback_2);
            curr_i2c->onReceive(on_receive_callback_2);
            break;
        }
    }

    is_confgured[i2c] = true;

    curr_i2c->setClock(frequency);
    return 0;
}

int I2CHandler::unset_configuration(uint8_t i2c) {
    TwoWire *curr_i2c = NULL;
    if(i2c >= I2C_COUNT) {
        return EINVAL;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ENODEV;
    }

    curr_i2c->end();
    is_confgured[i2c] = false;
    is_controller[i2c] = false;

    return 0;
}

bool I2CHandler::is_configured(uint8_t i2c) {
    return is_confgured[i2c];
}

int I2CHandler::handle_controller_read(TwoWire *i2c, uint8_t *data, size_t size) {
    if(i2c->available() > 0) {
        for (int i = 0; i < size; i++)
        {
            data[i] = i2c->read();
        }
    }

}

TesterError I2CHandler::handle_peripheral_read(uint8_t i2c, uint8_t *data) {
    if(i2c >= I2C_COUNT) {
        return ERROR_INVALID_ARGUMENT;
    }

    data = (uint8_t*) malloc(sizeof(uint8_t) * REGISTER_COUNT);

    if (data == NULL){
        return ERROR_NO_MEM;
    }

    memcpy(data, data_register[i2c], sizeof(uint8_t) * REGISTER_COUNT);
    return 0;
}

// CALLBACKS
void I2CHandler::on_receive_callback_0(int numBytes) {
    if (numBytes > 2)
    {
        return;
    }

    uint8_t register_index = Wire.read();
    if (register_index >= REGISTER_COUNT)
    {
        return;
    }
    data_register[0][register_index] = Wire.read();
}

void I2CHandler::on_receive_callback_1(int numBytes) {
    if (numBytes > 2)
    {
        return;
    }

    uint8_t register_index = Wire1.read();
    if (register_index >= REGISTER_COUNT)
    {
        return;
    }
    data_register[1][register_index] = Wire1.read();
}

void I2CHandler::on_receive_callback_2(int numBytes) {
    if (numBytes > 2)
    {
        return;
    }

    uint8_t register_index = Wire2.read();
    if (register_index >= REGISTER_COUNT)
    {
        return;
    }
    data_register[2][register_index] = Wire2.read();
}

void I2CHandler::on_request_callback_0() {
    Wire.write(data_register[0], 2);
}

void I2CHandler::on_request_callback_1() {
    Wire1.write(data_register[1], 2);
}

void I2CHandler::on_request_callback_2() {
    Wire2.write(data_register[2], 2);
}
//END OF CALLBACKS