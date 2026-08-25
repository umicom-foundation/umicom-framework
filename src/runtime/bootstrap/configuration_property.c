/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_property.c
 *
 * PURPOSE:
 *   Create bounded configuration properties with source precedence.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/configuration_property.h"


#include <string.h>
UmiStatus umi_bootstrap_configuration_property_init(
    UmiBootstrapConfigurationProperty *property,
    const char *key,
    const char *value,
    int32_t precedence,
    bool secret) {
    UmiStatus status;
    if (property == NULL || !umi_bootstrap_id_valid(key) || value == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(property, 0, sizeof(*property));
    status = umi_bootstrap_copy_text(property->key, sizeof(property->key), key);
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(property->value, sizeof(property->value), value);
    if (status != UMI_STATUS_OK) return status;
    property->precedence = precedence;
    property->secret = secret;
    return UMI_STATUS_OK;
}
