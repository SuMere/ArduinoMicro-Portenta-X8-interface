#pragma once

#include <Arduino.h>

#define LOGIN_TIMEOUT 60000
#define COMMAND_TIMEOUT 10000
#define LOGIN_STRING "login:"

#define PSW_STRING "fio"
#define USR_STRING "fio"

#define PSW_PROMPT "Password:"
#define IN_SHELL_PROMPT "@dev-portenta"

typedef enum {
    ERROR_INIT_FAILED,
    ERROR_TIMEOUT,
    ERROR_INVALID_STATE,
    ERROR_INVALID_COMMAND,
    ERROR_INVALID_ARGUMENT,
    ERROR_INVALID_RESPONSE,
    ERROR_TEST_FAILED,
    ERROR_NO_MEM,
    ERROR_INTERNAL,
    NO_ERROR = 99
} TesterError;