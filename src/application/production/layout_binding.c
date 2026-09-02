/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/layout_binding.c
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
#include "umicom/application/production/layout_binding.h"

#include <string.h>

#include "umicom/application/suite_layout/runtime.h"

/*
 * Provide the application production layout bindings build operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_production_layout_bindings_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionLayoutBindings *out_bindings)
{
    UmiApplicationSuiteLayoutRuntime runtime;
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
    if (binding->experience->layout_count >
        UMI_APPLICATION_PRODUCTION_MAX_LAYOUTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_bindings, 0, sizeof(*out_bindings));
    status = umi_product_application_adoption_layout_load(
        binding->adoption, &runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < binding->experience->layout_count; ++index) {
        UmiApplicationProductionLayoutBinding *entry =
            &out_bindings->entries[out_bindings->count];
        entry->layout = &binding->experience->layouts[index];
        entry->default_layout =
            strcmp(entry->layout->layout_id,
                   binding->experience->default_layout_id) == 0;
        status = umi_application_suite_layout_runtime_select(
            &runtime, entry->layout->layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            entry->projected_window_count = runtime.active_layout.window_count;
            entry->projection_ready =
                entry->projected_window_count > 0U;
            out_bindings->projected_count +=
                (size_t)entry->projection_ready;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_bindings->missing_projection_count += 1U;
        }
        /* Apply this operation only while the related capability or state is available. */
        if (!entry->projection_ready)
            out_bindings->missing_projection_count +=
                (status == UMI_STATUS_OK) ? 1U : 0U;
        out_bindings->count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Find application production layout bindings while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationProductionLayoutBinding *
umi_application_production_layout_bindings_find(
    const UmiApplicationProductionLayoutBindings *bindings,
    const char *layout_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || layout_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bindings->count; ++index)
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (bindings->entries[index].layout != NULL &&
            strcmp(bindings->entries[index].layout->layout_id, layout_id) == 0)
            return &bindings->entries[index];
    return NULL;
}

