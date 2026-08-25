/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/environment_condition.c
 *
 * PURPOSE:
 *   Evaluate environment-style key/value overlays supplied to bootstrap.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/environment_condition.h"
#include "umicom/runtime/bootstrap/configuration_overlay.h"


#include <string.h>
bool umi_bootstrap_environment_condition_match(
    const UmiBootstrapPropertySet *environment,
    const char *key,
    const char *expected_value) {
    const UmiBootstrapConfigurationProperty *property =
        umi_bootstrap_property_set_find(environment, key);
    if (property == NULL) return false;
    return expected_value == NULL || strcmp(property->value, expected_value) == 0;
}
