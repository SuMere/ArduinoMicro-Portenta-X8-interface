#include "i2c_handler.h"

#include "at_handler.h"

i2c_handler_data_t I2CHandler::i2c_data[I2C_COUNT] = {
    {&Wire,  {0}, 0, false, false, 0},
    {&Wire1, {0}, 0, false, false, 0},
    {&Wire2, {0}, 0, false, false, 0}
};

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

    i2c_handler_data_t *curr = &i2c_data[bus_number];
    switch (parser.args.size())
    {
    case 1: // GET REGISTER STATUS AS PERIPHERAL
    {
        if(curr->is_controller){
            return write_error_message(srv, "Selected bus number should be configured as peripheral");
        }
        String message;
        for (int i = 0; i < REGISTER_COUNT; i++){
            message += "0x"+String(curr->data_register[i], HEX)+ " ";
        }
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

	for (int a=0; a<0x80; ++a) {
		if (a == 0) {
			message += "00: ";
		} else if (!(a % 16)) {
			message += "\n" + String(a, HEX) + ": ";
		}
		if (a < 0x08 || a > 0x77) {
			message += "   ";
		} else if (scan_res[a] == 1) {
			if (a < 16) message += "0";
			message += String(a, HEX) + " ";
		} else {
			message += "-- ";
		}
	}
	message += "\n";

        return write_ok_message(srv, message.c_str());
    }
    case 4: // READ AS CONTROLLER
    {
        uint8_t address = (uint8_t)strtol(parser.args[1].c_str(), NULL, 16);
        uint8_t reg_addr = 0;
        bool has_reg_addr = (parser.args[2].length() != 0);
        if (address < 0 || address > 127){
            return write_error_message(srv, "Invalid I2C address");
        }
        if(has_reg_addr){
            reg_addr = (uint8_t)strtol(parser.args[2].c_str(), NULL, 16);
        }
        data_size = parser.args[3] != "" ? atoi(parser.args[3].c_str()) : 1;
        data = (uint8_t *)malloc(data_size);
        if(curr->is_controller == false){
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

    i2c_handler_data_t *curr = &i2c_data[bus_number];
    if(curr->is_controller == false){
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

chAT::CommandStatus I2CHandler::handle_test(chAT::Server &srv, chAT::ATParser &parser)
{
    String message = "\n";
    message += "AT+I2C_CFG=<bus number>,<bus speed>,<peripheral address (optional)> - Configure I2C bus, if <peripheral address> is specified device is configured as peripheral\n";
    message += "AT+I2C_CFG?<bus number> - Read I2C bus configuration\n";
    message += "AT+I2C_CFG=<bus number> - Clears I2C bus configuration\n";
    message += "AT+I2C?<bus number> - Prints I2C register when configured as peripheral \n";
    message += "AT+I2C?<bus number>,SCAN - Scans I2C bus for devices connected when configured as controller\n";
    message += "AT+I2C?<bus number>,<address>,<register address(optional)>,<data size> - Reads <data size> bytes from device <address> at <register address> (if specified)\n";
    message += "AT+I2C=<bus number>,<address>,<data>,<data size>,<register address(optional) - Writes <data> to device <address> at <register address> (if specified)\n";

    return write_ok_message(srv, message.c_str());
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

    i2c_handler_data_t *curr = &i2c_data[bus_number];
    String message = "";
    if(curr->is_configured == false){
        message = "I2C bus "+String(bus_number)+" not configured";
    }else{
        message = "I2C bus "+String(bus_number)+" configured as ";
        if(curr->is_controller){
            message += "controller @ "+String(curr->controller_frequency)+"kHz";
        }else{
            message += "peripheral with address 0x"+String(curr->peripheral_address, HEX);
            message += "\nData registers Status:\n";
            for (int i = 0; i < REGISTER_COUNT; i++){
                message += "["+String(i)+"] 0x"+String(curr->data_register[i], HEX)+ "\n";
            }
        }
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
    bool is_controller = parser.args[2].length() == 0 ? true : false;
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

int I2CHandler::i2c_read(uint8_t bus_number, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr) {
    if(bus_number >= I2C_COUNT) {
        return -EINVAL;
    }

    i2c_handler_data_t *curr = &i2c_data[bus_number];

    if(curr->is_configured == false) {
        return -EPERM;
    }

    if(curr->is_controller == false){
        return -EPERM;
    }

    curr->bus->beginTransmission(address);
    if(register_address != 0) {
        curr->bus->requestFrom(address, size);
        if (has_reg_addr) {
            curr->bus->write(register_address);
        }
    }

    if(curr->bus->endTransmission(true) != 0) {
        return -EIO;
    }

    return handle_controller_read(curr->bus, data, size);
}

int I2CHandler::i2c_write(uint8_t bus_number, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr) {
    if (bus_number >= I2C_COUNT)
    {
        return -EINVAL;
    }

    i2c_handler_data_t *curr = &i2c_data[bus_number];

    i2c_handler_data_t *curr = &i2c_data[bus_number];

    if(curr->is_configured == false ||
       curr->is_controller == false)
    {
        return -EPERM;
    }

    curr->bus->beginTransmission(address);
    if (has_reg_addr) {
        curr->bus->write(register_address);
    }

    for(size_t i = 0; i < size; i++) {
        curr->bus->write(data[i]);
    }
    if(curr->bus->endTransmission() != 0) {
        return -EIO;
    }

    return 0;
}

int I2CHandler::i2c_scan(uint8_t bus_number, uint8_t *addresses) {
    if (bus_number >= I2C_COUNT)
    {
        return -EINVAL;
    }

    i2c_handler_data_t *curr = &i2c_data[bus_number];

    if (curr->is_configured == false ||
        curr->is_controller == false)
    {
        return -EPERM;
    }

    for (uint8_t i = 8; i < 119; i++)
    {
        curr->bus->beginTransmission(i);
        if (curr->bus->endTransmission() == 0)
        {
            addresses[i] = 1;
        }
    }

    return 0;
}

int I2CHandler::set_configuration(uint8_t bus_number, uint8_t address, uint32_t frequency, bool is_controller) {
    if(bus_number >= I2C_COUNT) {
        return -EINVAL;
    }

    i2c_handler_data_t *curr = &i2c_data[bus_number];

    if(is_controller){
        curr->bus->begin();
        curr->bus->setClock(frequency);
        curr->is_controller = true;
        curr->peripheral_address = 0;
        curr->controller_frequency = frequency;
    }else{
        curr->bus->begin(address);
        curr->is_controller = false;
        curr->peripheral_address = address;
        curr->controller_frequency = 0;
        switch (bus_number)
        {
        case 0:
            curr->bus->onRequest(on_request_callback_0);
            curr->bus->onReceive(on_receive_callback_0);
            break;
        case 1:
            curr->bus->onRequest(on_request_callback_1);
            curr->bus->onReceive(on_receive_callback_1);
            break;
        case 2:
            curr->bus->onRequest(on_request_callback_2);
            curr->bus->onReceive(on_receive_callback_2);
            break;
        }
    }

    curr->data_register[0] = 0xff;
    curr->data_register[1] = 0xff;

    curr->is_configured = true;
    return 0;
}

int I2CHandler::unset_configuration(uint8_t bus_number) {
    if(bus_number >= I2C_COUNT) {
        return -EINVAL;
    }

    i2c_handler_data_t *curr = &i2c_data[bus_number];

    curr->bus->end();
    curr->is_configured = false;
    curr->is_controller = false;
    curr->controller_frequency = 0;
    curr->peripheral_address = 0;

    return 0;
}

int I2CHandler::handle_controller_read(TwoWire *i2c, uint8_t *data, size_t size) {
    if(i2c->available() > 0) {
        for (int i = 0; i < size; i++)
        {
            data[i] = i2c->read();
        }
    }

    return 0;
}

// CALLBACKS

void I2CHandler::on_receive_callback(uint8_t bus_number, int numBytes) {
    Serial.println("on_receive_callback");
    Serial.println(numBytes);
    if (numBytes < 2)
    {
        return;
    }

    i2c_handler_data_t *curr = &i2c_data[bus_number];
    uint8_t register_index = curr->bus->read();
    if (register_index >= REGISTER_COUNT)
    {
        return;
    }

    curr->data_register[register_index] = curr->bus->read();
}

void I2CHandler::on_receive_callback_0(int numBytes) {
	return on_receive_callback(0, numBytes);
}

void I2CHandler::on_receive_callback_1(int numBytes) {
	return on_receive_callback(1, numBytes);
}

void I2CHandler::on_receive_callback_2(int numBytes) {
	return on_receive_callback(2, numBytes);
}

void I2CHandler::on_request_callback(uint8_t bus_number) {
    i2c_handler_data_t *curr = &i2c_data[bus_number];
    uint8_t register_index = curr->bus->read();
    if (register_index >= REGISTER_COUNT)
    {
        register_index = 0;
    }
    curr->bus->write(curr->data_register[register_index]);
}

void I2CHandler::on_request_callback_0() {
	return on_request_callback(0);
}

void I2CHandler::on_request_callback_1() {
	return on_request_callback(1);
}

void I2CHandler::on_request_callback_2() {
	return on_request_callback(2);
}

//END OF CALLBACKS
