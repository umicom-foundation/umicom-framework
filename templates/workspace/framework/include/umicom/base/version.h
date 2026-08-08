#ifndef UMICOM_BASE_VERSION_H
#define UMICOM_BASE_VERSION_H

#include <stdint.h>

#define UMICOM_FRAMEWORK_VERSION_MAJOR 0
#define UMICOM_FRAMEWORK_VERSION_MINOR 4
#define UMICOM_FRAMEWORK_VERSION_PATCH 2
#define UMICOM_FRAMEWORK_VERSION_STRING "0.4.2"
#define UMICOM_FRAMEWORK_ABI_VERSION 1U

typedef struct UmiVersion {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
} UmiVersion;

#endif
