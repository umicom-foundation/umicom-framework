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
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
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
