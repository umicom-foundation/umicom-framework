/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/configuration_overlay.h
 *
 * PURPOSE:
 *   Merge configuration overlays while preserving highest-precedence values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_OVERLAY_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_OVERLAY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap property set from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_property_set_init(UmiBootstrapPropertySet *set);
/**
 * Find bootstrap property set while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiBootstrapConfigurationProperty *umi_bootstrap_property_set_find(
    const UmiBootstrapPropertySet *set,
    const char *key);
/**
 * Perform bootstrap configuration overlay through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_bootstrap_configuration_overlay_apply(
    UmiBootstrapPropertySet *set,
    const UmiBootstrapConfigurationProperty *property);

#ifdef __cplusplus
}
#endif

#endif
