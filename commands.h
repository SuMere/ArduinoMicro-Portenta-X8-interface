#ifndef WIFI_COMMANDS_H
#define WIFI_COMMANDS_H

enum ip_type {
   IP_ADDR = 0,
   GATEWAY_ADDR,
   NETMASK_ADDR,
   DNS1_ADDR,
   DNS2_ADDR
};

enum file_op {
   WIFI_FILE_DELETE = 0,
   WIFI_FILE_WRITE,
   WIFI_FILE_READ,
   WIFI_FILE_APPEND
};

#define _AT                      "AT"
#define _ENDL                    "\r\n" 

#define _GPIO                 "+GPIO"
#define _ADC                  "+ADC"
#define _DAC                  "+DAC"
#define _PWM                  "+PWM"
#define _CAN                  "+CAN"
#define _I2C                  "+I2C"
#define _SPI                  "+SPI"
#define _UART                 "+UART"
#define _I2S                  "+I2S"

#define _HELP                 "+HELP"

#define _REPORT               "+REPORT"

#define CMD(x)             _AT x _ENDL
#define PROMPT(x)          x ":"
#define CMD_WRITE(x)       _AT x "="
#define CMD_READ(x)        _AT x "?" _ENDL

#define START_CLIENT_SERVER_SOCK 1000
#define START_SSL_CLIENT_SOCK    2000

#endif
