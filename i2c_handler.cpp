#include "i2c_handler.h"

#define REGISTER_NUM 2

uint8_t curr_i2c_config = 0;
uint8_t peripheral_received = 0;
uint8_t register0[REGISTER_NUM];
uint8_t register1[REGISTER_NUM];
uint8_t register2[REGISTER_NUM];



// CALLBACKS

void onRequestCallback0() {
    Wire.write(register0, 2);
}

void onRequestCallback1() {
    Wire1.write(register1, 2);
}

void onRequestCallback2() {
    Wire2.write(register2, 2);
}

void onReceiveCallback0(int numBytes) {
    if (numBytes > 2)
    {
        return;
    }

    uint8_t register_index = Wire.read();
    if (register_index >= REGISTER_NUM)
    {
        return;
    }
    register0[register_index] = Wire.read();
    peripheral_received |= I2C0_CONFIG;
}

void onReceiveCallback1(int numBytes) {
    if (numBytes > 2)
    {
        return;
    }

    uint8_t register_index = Wire1.read();
    if (register_index >= REGISTER_NUM)
    {
        return;
    }
    register1[register_index] = Wire1.read();
    peripheral_received |= I2C1_CONFIG;
}

void onReceiveCallback2(int numBytes) {
    if (numBytes > 2)
    {
        return;
    }

    uint8_t register_index = Wire2.read();
    if (register_index >= REGISTER_NUM)
    {
        return;
    }
    register2[register_index] = Wire2.read();
    peripheral_received |= I2C2_CONFIG;
}

//END OF CALLBACKS

void set_i2c_config(uint8_t config){
    curr_i2c_config |= config;
}

uint8_t get_i2c_config(){
    return curr_i2c_config;
}

void clear_config(uint8_t config){
    curr_i2c_config &= config;
}

I2CHandler::I2CHandler() {}

I2CHandler::~I2CHandler() {}

TesterError I2CHandler::i2cRead(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr) {
    TesterError opStatus = NO_ERROR;
    TwoWire *curr_i2c = NULL;
    uint8_t tmp_cfg = get_i2c_config();
    bool is_controller = true;

    switch(i2c)
    {
        case 0:
            if(!(tmp_cfg & I2C0_CONFIG)){
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire;
            if(tmp_cfg & I2C0_PERIPHERAL) {
                is_controller = false;
            }
            break;
        case 1:
            if(!(tmp_cfg & I2C1_CONFIG)){
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire1;
            if(tmp_cfg & I2C1_PERIPHERAL) {
                is_controller = false;
            }
            break;
        case 2:
            if(!(tmp_cfg & I2C2_CONFIG)){
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire2;
            if(tmp_cfg & I2C2_PERIPHERAL) {
                is_controller = false;
            }
            break;
        default:
            return ERROR_INVALID_ARGUMENT;
    }

    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }

    if(is_controller){
        Serial.println("STEP 3.1");
        curr_i2c->beginTransmission(address);
        Serial.println("STEP 3.2");
        if(register_address != 0) {
            Serial.println("STEP 3.2.1");
            Serial.println(size);
            Serial.println(address);
            curr_i2c->requestFrom(address, size);
            if (has_reg_addr) {
                Serial.println("STEP 3.2.2");
                curr_i2c->write(register_address);
            } 
        }
        Serial.println("STEP 3.3");
        if(curr_i2c->endTransmission(true) == 0) {
            Serial.println("STEP 3.3.1");
            Serial.flush();
            opStatus = handleControllerRead(curr_i2c, data, size);
        }else{
            Serial.println("STEP 3.3.2");
            opStatus = ERROR_INVALID_RESPONSE;
        }
    }else{
        opStatus = handlePeripheralRead(i2c, data);
    }

    return opStatus;
}

TesterError I2CHandler::i2cWrite(uint8_t i2c, uint8_t address, uint8_t register_address, uint8_t *data, size_t size, bool has_reg_addr) {
    TwoWire *curr_i2c = NULL;
    uint8_t tmp_cfg = get_i2c_config();

    switch(i2c)
    {
        case 0:
            if(!(tmp_cfg & I2C0_CONFIG) || tmp_cfg & I2C0_PERIPHERAL) {
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire;
            break;
        case 1:
            if(!(tmp_cfg & I2C1_CONFIG) || tmp_cfg & I2C1_PERIPHERAL) {
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire1;
            break;
        case 2:
            if(!(tmp_cfg & I2C2_CONFIG) || tmp_cfg & I2C2_PERIPHERAL) {
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire2;
            break;
        default:
            return ERROR_INVALID_ARGUMENT;
    }

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

TesterError I2CHandler::i2cScan(uint8_t i2c, uint8_t *addresses) {
    TwoWire *curr_i2c = NULL;
    uint8_t tmp_cfg = get_i2c_config();
    switch(i2c)
    {
        case 0:
            if(!(tmp_cfg & I2C0_CONFIG) || tmp_cfg & I2C0_PERIPHERAL) {
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire;
            break;
        case 1:
            if(!(tmp_cfg & I2C1_CONFIG) || tmp_cfg & I2C1_PERIPHERAL) {
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire1;
            break;
        case 2:
            if(!(tmp_cfg & I2C2_CONFIG) || tmp_cfg & I2C2_PERIPHERAL) {
                return ERROR_INVALID_STATE;
            }
            curr_i2c = &Wire2;
            break;
        default:
            return ERROR_INVALID_ARGUMENT;
    }
    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }
    for (uint8_t i = 3; i < 119; i++)
    {
        Serial.flush();
        curr_i2c->beginTransmission(i);
        if (curr_i2c->endTransmission() == 0)
        {
            addresses[i-3] = 1;
            return NO_ERROR;
        }
    }
    return NO_ERROR;
}

TesterError I2CHandler::setConfiguration(uint8_t i2c, uint8_t address, uint32_t frequency, bool is_controller) {
    TwoWire *curr_i2c = NULL;
    uint8_t cfg = 0;

    switch (i2c)
    {
    case 0:
        curr_i2c = &Wire;
        cfg |= I2C0_CONFIG;
        if(!is_controller) {
            cfg |= I2C0_PERIPHERAL;
            if(curr_i2c != NULL) {
                curr_i2c->onRequest(onRequestCallback0);
                curr_i2c->onReceive(onReceiveCallback0);
            }
        }
        break;
    case 1:
        curr_i2c = &Wire1;
        cfg |= I2C1_CONFIG;
        if(!is_controller) {
            cfg |= I2C1_PERIPHERAL;
            if (curr_i2c != NULL)
            {
                curr_i2c->onRequest(onRequestCallback1);
                curr_i2c->onReceive(onReceiveCallback1);
            }
        }
        break;
    case 2:
        curr_i2c = &Wire2;
        cfg |= I2C2_CONFIG;
        if(!is_controller) {
            cfg |= I2C2_PERIPHERAL;
            if (curr_i2c != NULL)
            {
                curr_i2c->onRequest(onRequestCallback2);
                curr_i2c->onReceive(onReceiveCallback2);
            }
        }
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }

    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }

    if(is_controller) {
        curr_i2c->begin();
    } else {
        curr_i2c->begin(address);
    }

    curr_i2c->setClock(frequency);

    set_i2c_config(cfg);
    return NO_ERROR;
}

TesterError I2CHandler::unsetConfiguration(uint8_t i2c) {
    TwoWire *curr_i2c = NULL;
    uint8_t cfg_mask = 0;

    switch (i2c)
    {
    case 0:
        curr_i2c = &Wire;
        cfg_mask &= (~I2C0_CONFIG) & (~I2C0_PERIPHERAL);
        break;
    case 1:
        curr_i2c = &Wire1;
        cfg_mask &= (~I2C1_CONFIG) & (~I2C1_PERIPHERAL);
        break;
    case 2:
        curr_i2c = &Wire2;
        cfg_mask &= (~I2C2_CONFIG) & (~I2C2_PERIPHERAL);
        break;
    default:
        return ERROR_INVALID_ARGUMENT;
    }

    if(!curr_i2c) {
        return ERROR_INIT_FAILED;
    }

    curr_i2c->end();
    clear_config(cfg_mask);
    return NO_ERROR;
}

bool I2CHandler::isConfigured(uint8_t i2c) {
    return true;
}

TesterError I2CHandler::handleControllerRead(TwoWire *i2c, uint8_t *data, size_t size) {
    if(i2c->available() > 0) {
        for (int i = 0; i < size; i++)
        {
            data[i] = i2c->read();
        }
    }

    return NO_ERROR;
}

TesterError I2CHandler::handlePeripheralRead(uint8_t i2c, uint8_t *data) {
    TesterError opStatus = NO_ERROR;
    data = (uint8_t*) malloc(sizeof(uint8_t) * REGISTER_NUM);
    if (data == NULL){
        return ERROR_NO_MEM;
    }
    
    if(i2c == 0) {
        if(peripheral_received & I2C0_CONFIG) {
            memcpy(data, register0, sizeof(uint8_t) * REGISTER_NUM);
        }else{
            opStatus = ERROR_INVALID_RESPONSE;
        }
    }else if(i2c == 1) {
        if(peripheral_received & I2C1_CONFIG) {
            memcpy(data, register1, sizeof(uint8_t) * REGISTER_NUM);
        }else{
            opStatus = ERROR_INVALID_RESPONSE;
        }
    }else if(i2c == 2) {
        if(peripheral_received & I2C2_CONFIG) {
            memcpy(data, register2, sizeof(uint8_t) * REGISTER_NUM);
        }else{
            opStatus = ERROR_INVALID_RESPONSE;
        }
    }else{
        opStatus = ERROR_INVALID_ARGUMENT;
    }
    return NO_ERROR;
}