/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_property.c
 *
 * PURPOSE:
 *   Implement the configuration property behavior for
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
 * File: src/runtime/bootstrap/configuration_property.c
 *
 * PURPOSE:
 *   Create bounded configuration properties with source precedence.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/configuration_property.h"


#include <string.h>
/*
 * Initialise bootstrap configuration property from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_bootstrap_configuration_property_init(
    UmiBootstrapConfigurationProperty *property,
    const char *key,
    const char *value,
    int32_t precedence,
    bool secret) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL || !umi_bootstrap_id_valid(key) || value == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(property, 0, sizeof(*property));
    status = umi_bootstrap_copy_text(property->key, sizeof(property->key), key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(property->value, sizeof(property->value), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    property->precedence = precedence;
    property->secret = secret;
    return UMI_STATUS_OK;
}
