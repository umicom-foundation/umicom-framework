/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/configuration_property.h
 *
 * PURPOSE:
 *   Create bounded configuration properties with source precedence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_PROPERTY_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_PROPERTY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap configuration property from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_bootstrap_configuration_property_init(
    UmiBootstrapConfigurationProperty *property,
    const char *key,
    const char *value,
    int32_t precedence,
    bool secret);

#ifdef __cplusplus
}
#endif

#endif
