/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/feature_binding.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/feature_binding.h"

#include <string.h>

/*
 * Provide the application production feature bindings build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_feature_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionFeatureBindings *out_bindings)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (binding->experience->feature_count >
        UMI_APPLICATION_PRODUCTION_MAX_FEATURES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_bindings, 0, sizeof(*out_bindings));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < binding->experience->feature_count; ++index) {
        UmiApplicationProductionFeatureBinding *entry =
            &out_bindings->entries[out_bindings->count];
        entry->feature = &binding->experience->features[index];
        entry->open =
            entry->feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED;
        entry->application_work =
            entry->open &&
            entry->feature->owner == UMI_EXPERIENCE_OWNER_APPLICATION;
        entry->external_adapter_work =
            entry->open &&
            entry->feature->owner == UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER;
        out_bindings->open_count += (size_t)entry->open;
        out_bindings->application_work_count +=
            (size_t)entry->application_work;
        out_bindings->external_adapter_work_count +=
            (size_t)entry->external_adapter_work;
        out_bindings->count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Find application production feature bindings while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionFeatureBinding *
umi_application_production_feature_bindings_find(
    const UmiApplicationProductionFeatureBindings *bindings,
    const char *feature_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || feature_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bindings->count; ++index)
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (bindings->entries[index].feature != NULL &&
            strcmp(bindings->entries[index].feature->feature_id,
                   feature_id) == 0)
            return &bindings->entries[index];
    return NULL;
}

