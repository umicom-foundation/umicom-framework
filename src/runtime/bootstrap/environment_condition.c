/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/environment_condition.c
 *
 * PURPOSE:
 *   Implement the environment condition behavior for
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
 * File: src/runtime/bootstrap/environment_condition.c
 *
 * PURPOSE:
 *   Evaluate environment-style key/value overlays supplied to bootstrap.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/environment_condition.h"
#include "umicom/runtime/bootstrap/configuration_overlay.h"


#include <string.h>
/*
 * Provide the bootstrap environment condition match operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_environment_condition_match(
    const UmiBootstrapPropertySet *environment,
    const char *key,
    const char *expected_value) {
    const UmiBootstrapConfigurationProperty *property =
        umi_bootstrap_property_set_find(environment, key);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL) return false;
    return expected_value == NULL || strcmp(property->value, expected_value) == 0;
}
