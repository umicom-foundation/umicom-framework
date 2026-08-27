/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/feature_binding.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/feature_binding.h"

#include <string.h>

UmiStatus umi_application_production_feature_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionFeatureBindings *out_bindings)
{
    size_t index;
    UmiStatus status;
    if (out_bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    if (binding->experience->feature_count >
        UMI_APPLICATION_PRODUCTION_MAX_FEATURES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_bindings, 0, sizeof(*out_bindings));
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

const UmiApplicationProductionFeatureBinding *
umi_application_production_feature_bindings_find(
    const UmiApplicationProductionFeatureBindings *bindings,
    const char *feature_id)
{
    size_t index;
    if (bindings == NULL || feature_id == NULL) return NULL;
    for (index = 0U; index < bindings->count; ++index)
        if (bindings->entries[index].feature != NULL &&
            strcmp(bindings->entries[index].feature->feature_id,
                   feature_id) == 0)
            return &bindings->entries[index];
    return NULL;
}

