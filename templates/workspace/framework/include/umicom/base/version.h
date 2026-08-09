/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/base/version.h
 *
 * PURPOSE:
 *   Publish the Framework semantic version and stable ABI version used by
 *   applications, modules, packages, diagnostics, and release validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BASE_VERSION_H
#define UMICOM_BASE_VERSION_H

#include <stdint.h>

#define UMICOM_FRAMEWORK_VERSION_MAJOR 0
#define UMICOM_FRAMEWORK_VERSION_MINOR 4
#define UMICOM_FRAMEWORK_VERSION_PATCH 4
#define UMICOM_FRAMEWORK_VERSION_STRING "0.4.4"
#define UMICOM_FRAMEWORK_ABI_VERSION 1U

typedef struct UmiVersion {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
} UmiVersion;

#endif
