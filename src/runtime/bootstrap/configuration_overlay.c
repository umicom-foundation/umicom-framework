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
/*
 * Initialise bootstrap property set from caller-provided values so later operations
 * receive a known state.
 */
void umi_bootstrap_property_set_init(UmiBootstrapPropertySet *set) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set != NULL) memset(set, 0, sizeof(*set));
}
/*
 * Find bootstrap property set while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiBootstrapConfigurationProperty *umi_bootstrap_property_set_find(
    const UmiBootstrapPropertySet *set,
    const char *key) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || key == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < set->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(set->items[i].key, key) == 0) return &set->items[i];
    return NULL;
}
/*
 * Perform bootstrap configuration overlay through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_bootstrap_configuration_overlay_apply(
    UmiBootstrapPropertySet *set,
    const UmiBootstrapConfigurationProperty *property) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || property == NULL || !umi_bootstrap_id_valid(property->key))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < set->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(set->items[i].key, property->key) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (property->precedence >= set->items[i].precedence) {
                set->items[i] = *property;
                ++set->revision;
            }
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_BOOTSTRAP_MAX_PROPERTIES) return UMI_STATUS_CAPACITY_EXCEEDED;
    set->items[set->count++] = *property;
    ++set->revision;
    return UMI_STATUS_OK;
}
