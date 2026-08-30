/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/property_condition.c
 *
 * PURPOSE:
 *   Implement the property condition behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/property_condition.c
 *
 * PURPOSE:
 *   Evaluate configuration-property equality and presence.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/property_condition.h"
#include "umicom/runtime/bootstrap/configuration_overlay.h"


#include <string.h>
bool umi_bootstrap_property_condition_match(
    const UmiBootstrapPropertySet *properties,
    const char *key,
    const char *expected_value) {
    const UmiBootstrapConfigurationProperty *property =
        umi_bootstrap_property_set_find(properties, key);
    if (property == NULL) return false;
    return expected_value == NULL || expected_value[0] == '\0' ||
           strcmp(property->value, expected_value) == 0;
}
