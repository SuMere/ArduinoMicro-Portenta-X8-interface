#include "i2c_handler.h"

#include "at_handler.h"

uint8_t I2CHandler::data_register[I2C_COUNT][REGISTER_COUNT] = {{0}};

I2CHandler::I2CHandler(CAtHandler *parent)
    : CmdHandler(parent) 
{
    parent->registerCommands("+I2C", this);
}

chAT::CommandStatus I2CHandler::handle_read(chAT::Server &srv, chAT::ATParser &parser)
{
    uint8_t *data = NULL;
    size_t data_size = 1;

    if (parser.args.size() != 1 &&
        parser.args.size() != 2 &&
        parser.args.size() != 4)
    {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    uint8_t bus_number = atoi(parser.args[0].c_str());
    if(bus_number < 0 || bus_number > I2C_COUNT){
        srv.write_response_prompt();
        srv.write_str("Invalid I2C bus number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    switch (parser.args.size())
    {
    case 1: // GET REGISTER STATUS AS PERIPHERAL
        if(is_controller[bus_number]){
            srv.write_response_prompt();
            srv.write_str("Selected bus number should be configured as peripheral");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }
        if(i2c_read(bus_number, 0, 0, data, REGISTER_COUNT, false) != NO_ERROR){
            srv.write_response_prompt();
            srv.write_str("Error reading I2C");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }
        srv.write_response_prompt();
        srv.write_str("I2C read result from device, data: ");
        for (int i = 0; i < data_size; i++){
            srv.write_str(String(data[i], HEX).c_str());
        }
        srv.write_line_end();
        free(data);
        return chAT::CommandStatus::OK;
    case 2: // SCAN
    {
        if(parser.args[1] != "SCAN"){
            srv.write_response_prompt();
            srv.write_str("Invalid argument");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }
        uint8_t scan_res[116] = {0};
        if(i2c_scan(bus_number, scan_res) != NO_ERROR){
            srv.write_response_prompt();
            srv.write_str("Error scanning I2C");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }
        srv.write_response_prompt();
        srv.write_str("I2C scan result: ");
        for (int i = 0; i < 116; i++){
            if(scan_res[i] == 1){
                srv.write_str(String(i+3, HEX).c_str());
            }
        }
        srv.write_line_end();
        return chAT::CommandStatus::OK;
    }
    case 4: // READ AS CONTROLLER
        uint8_t address = (uint8_t)strtol(parser.args[1].c_str(), NULL, 16);
        uint8_t reg_addr = 0;
        bool has_reg_addr = (parser.args[2] != "");
        if (address < 0 || address > 127){
            srv.write_response_prompt();
            srv.write_str("Invalid I2C address");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }
        if(has_reg_addr){
            reg_addr = (uint8_t)strtol(parser.args[2].c_str(), NULL, 16);
        }
        data_size = parser.args[3] != "" ? atoi(parser.args[3].c_str()) : 1;
        data = (uint8_t *)malloc(data_size);
        if(is_controller[bus_number] == false){
            srv.write_response_prompt();
            srv.write_str("Selected bus number should be configured as controller");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }
        if(i2c_read(bus_number, address, reg_addr, data, data_size, has_reg_addr) != NO_ERROR){
            srv.write_response_prompt();
            srv.write_str("Error reading I2C");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }
        String message ="I2C read result from device "+String(address, HEX);
        if(has_reg_addr){
            message += ", register: "+String(reg_addr, HEX);
        }
        message += ", data: ";
        srv.write_response_prompt();
        srv.write_str(message.c_str());
        for (int i = 0; i < data_size; i++){
            srv.write_str((String(data[i], HEX)+"-").c_str());
        }
        srv.write_line_end();
        free(data);
        return chAT::CommandStatus::OK;
    }
}

chAT::CommandStatus I2CHandler::handle_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 4) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    uint8_t bus_number = atoi(parser.args[0].c_str());
    if(bus_number < 0 || bus_number > I2C_COUNT){
        srv.write_response_prompt();
        srv.write_str("Invalid I2C bus number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }
    if(is_controller[bus_number] == false){
        srv.write_response_prompt();
        srv.write_str("Selected bus number should be configured as controller");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    uint8_t address = (uint8_t)strtol(parser.args[1].c_str(), NULL, 16);
    if (address < 0 || address > 127){
        srv.write_response_prompt();
        srv.write_str("Invalid I2C address");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    uint8_t data = (uint8_t)strtol(parser.args[2].c_str(), NULL, 16);
    size_t data_size = parser.args[2].length();
    uint8_t reg_addr = 0;
    bool has_reg_addr = (parser.args[3] != "");
    if(has_reg_addr){
        reg_addr = (uint8_t)strtol(parser.args[3].c_str(), NULL, 16);
    }

    if(i2c_write(bus_number, address, reg_addr, &data, data_size, has_reg_addr) != NO_ERROR){
        srv.write_response_prompt();
        srv.write_str("Error writing I2C");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    return chAT::CommandStatus::OK;
}

chAT::CommandStatus I2CHandler::handle_cfg_read(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    uint8_t bus_number = atoi(parser.args[0].c_str());
    if(bus_number < 0 || bus_number > I2C_COUNT){
        srv.write_response_prompt();
        srv.write_str("Invalid I2C bus number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    if(is_confgured[bus_number] == false){
        srv.write_response_prompt();
        srv.write_str(("I2C bus "+String(bus_number)+" not configured").c_str());
        srv.write_line_end();
        return chAT::CommandStatus::OK;
    }

    String message = "I2C bus "+String(bus_number)+" configured as ";
    if(is_controller[bus_number]){
        message += "controller";
    }else{
        message += "peripheral";
    }

    srv.write_response_prompt();
    srv.write_str(message.c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;
}

chAT::CommandStatus I2CHandler::handle_cfg_write(chAT::Server &srv, chAT::ATParser &parser)
{
    if (parser.args.size() != 1 && parser.args.size() != 3) {
        srv.write_response_prompt();
        srv.write_str("Invalid number of arguments");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    uint8_t bus_number = atoi(parser.args[0].c_str());
    if(bus_number < 0 || bus_number > I2C_COUNT){
        srv.write_response_prompt();
        srv.write_str("Invalid I2C bus number");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }

    if(parser.args.size() == 1) //CLEAR CONFIGURATION
    {
        if(unset_configuration(bus_number) != NO_ERROR){
            srv.write_response_prompt();
            srv.write_str("Error clearing I2C bus configuration");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }

        srv.write_response_prompt();
        srv.write_str(("I2C bus "+String(bus_number)+" configuration cleared").c_str());
        srv.write_line_end();
        return chAT::CommandStatus::OK;
    }

    uint8_t address = 0;
    uint32_t bus_speed = atoi(parser.args[1].c_str());
    bool is_controller = parser.args[2].c_str() == "" ? true : false;
    if(is_controller == false){
        address = (uint8_t)strtol(parser.args[2].c_str(), NULL, 16);
        if (address < 0 || address > 127){
            srv.write_response_prompt();
            srv.write_str("Invalid I2C address");
            srv.write_line_end();
            return chAT::CommandStatus::ERROR;
        }
    }

    if(set_configuration(bus_number, address, bus_speed, is_controller) != NO_ERROR){
        srv.write_response_prompt();
        srv.write_str("Error configuring I2C bus");
        srv.write_line_end();
        return chAT::CommandStatus::ERROR;
    }
    srv.write_response_prompt();
    srv.write_str(("I2C bus "+String(bus_number)+" configured").c_str());
    srv.write_line_end();
    return chAT::CommandStatus::OK;

}

TesterError I2CHandler::i2c_read(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr) {
    TesterError opStatus = NO_ERROR;
    TwoWire *curr_i2c = NULL;

    if(i2c >= I2C_COUNT) {
        return ERROR_INVALID_ARGUMENT;
    }

    if(is_confgured[i2c] == false) {
        return ERROR_INVALID_STATE;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }

    if(is_controller[i2c]){
        curr_i2c->beginTransmission(address);
        if(register_address != 0) {
            curr_i2c->requestFrom(address, size);
            if (has_reg_addr) {
                curr_i2c->write(register_address);
            } 
        }

        if(curr_i2c->endTransmission(true) == 0) {
            opStatus = handle_controller_read(curr_i2c, data, size);
        }else{
            opStatus = ERROR_INVALID_RESPONSE;
        }
    }else{
        opStatus = handle_peripheral_read(i2c, data);
    }

    return opStatus;
}

TesterError I2CHandler::i2c_write(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr) {
    TwoWire *curr_i2c = NULL;
    
    if (i2c >= I2C_COUNT)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (is_confgured[i2c] == false || is_controller[i2c] == false)
    {
        return ERROR_INVALID_STATE;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }

    curr_i2c->beginTransmission(address);
    if (has_reg_addr) {
        curr_i2c->write(register_address);
    }

    for(size_t i = 0; i < size; i++) {
        curr_i2c->write(data[i]);
    }
    if(curr_i2c->endTransmission() != 0) {
        return ERROR_INVALID_RESPONSE;
    }

    return NO_ERROR;
}

TesterError I2CHandler::i2c_scan(uint8_t i2c, uint8_t *addresses) {
    TwoWire *curr_i2c = NULL;

    if (i2c >= I2C_COUNT)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if(is_confgured[i2c] == false || is_controller[i2c] == false) {
        return ERROR_INVALID_STATE;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }

    for (uint8_t i = 3; i < 119; i++)
    {
        curr_i2c->beginTransmission(i);
        if (curr_i2c->endTransmission() == 0)
        {
            addresses[i-3] = 1;
            return NO_ERROR;
        }
    }
    return NO_ERROR;
}

TesterError I2CHandler::set_configuration(uint8_t i2c, uint8_t address, uint32_t frequency, bool is_controller) {
    TwoWire *curr_i2c = NULL;

    if(i2c >= I2C_COUNT) {
        return ERROR_INVALID_ARGUMENT;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }

    if(is_controller){
        curr_i2c->begin();
        this->is_controller[i2c] = true;
    }else{
        curr_i2c->begin(address);
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
    return NO_ERROR;
}

TesterError I2CHandler::unset_configuration(uint8_t i2c) {
    TwoWire *curr_i2c = NULL;
    if(i2c >= I2C_COUNT) {
        return ERROR_INVALID_ARGUMENT;
    }

    curr_i2c = i2c_array[i2c];

    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }

    curr_i2c->end();
    is_confgured[i2c] = false;
    is_controller[i2c] = false;

    return NO_ERROR;
}

bool I2CHandler::is_configured(uint8_t i2c) {
    return is_confgured[i2c];
}

TesterError I2CHandler::handle_controller_read(TwoWire *i2c, uint8_t *data, size_t size) {
    if(i2c->available() > 0) {
        for (int i = 0; i < size; i++)
        {
            data[i] = i2c->read();
        }
    }

    return NO_ERROR;
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

    return NO_ERROR;
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