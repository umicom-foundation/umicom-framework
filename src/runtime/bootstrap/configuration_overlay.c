/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_overlay.c
 *
 * PURPOSE:
 *   Implement the configuration overlay behavior for
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
 * File: src/runtime/bootstrap/configuration_overlay.c
 *
 * PURPOSE:
 *   Merge configuration overlays while preserving highest-precedence values.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/configuration_overlay.h"


#include <string.h>
void umi_bootstrap_property_set_init(UmiBootstrapPropertySet *set) {
    if (set != NULL) memset(set, 0, sizeof(*set));
}
const UmiBootstrapConfigurationProperty *umi_bootstrap_property_set_find(
    const UmiBootstrapPropertySet *set,
    const char *key) {
    size_t i;
    if (set == NULL || key == NULL) return NULL;
    for (i = 0U; i < set->count; ++i)
        if (strcmp(set->items[i].key, key) == 0) return &set->items[i];
    return NULL;
}
UmiStatus umi_bootstrap_configuration_overlay_apply(
    UmiBootstrapPropertySet *set,
    const UmiBootstrapConfigurationProperty *property) {
    size_t i;
    if (set == NULL || property == NULL || !umi_bootstrap_id_valid(property->key))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < set->count; ++i) {
        if (strcmp(set->items[i].key, property->key) == 0) {
            if (property->precedence >= set->items[i].precedence) {
                set->items[i] = *property;
                ++set->revision;
            }
            return UMI_STATUS_OK;
        }
    }
    if (set->count >= UMI_BOOTSTRAP_MAX_PROPERTIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    set->items[set->count++] = *property;
    ++set->revision;
    return UMI_STATUS_OK;
}
