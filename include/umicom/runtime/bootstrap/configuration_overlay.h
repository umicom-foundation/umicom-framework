/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/configuration_overlay.h
 *
 * PURPOSE:
 *   Merge configuration overlays while preserving highest-precedence values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_OVERLAY_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_OVERLAY_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


void umi_bootstrap_property_set_init(UmiBootstrapPropertySet *set);
const UmiBootstrapConfigurationProperty *umi_bootstrap_property_set_find(
    const UmiBootstrapPropertySet *set,
    const char *key);
UmiStatus umi_bootstrap_configuration_overlay_apply(
    UmiBootstrapPropertySet *set,
    const UmiBootstrapConfigurationProperty *property);

#ifdef __cplusplus
}
#endif

#endif
