/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/export_catalogue.c
 *
 * PURPOSE:
 *   Catalogue public CMake targets exported by an installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/sdk/export_catalogue.h"
#include "sdk_internal.h"

/*
 * Initialise sdk export catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_sdk_export_catalogue_init(UmiSdkExportCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) (void)memset(catalogue, 0, sizeof(*catalogue));
}

/*
 * Find sdk export catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSdkExportedTarget *umi_sdk_export_catalogue_find(
    const UmiSdkExportCatalogue *catalogue,
    const char *component_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || component_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->targets[index].component_id, component_id) == 0) {
            return &catalogue->targets[index];
        }
    }
    return NULL;
}

/* Add sdk export catalogue only after its inputs and available capacity have been checked. */
UmiStatus umi_sdk_export_catalogue_add(UmiSdkExportCatalogue *catalogue,
                                           const char *component_id,
                                           const char *target_name,
                                           int required)
{
    UmiSdkExportedTarget *target;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || !umi_sdk_text(component_id) ||
        !umi_sdk_text(target_name)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_SDK_EXPORT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_sdk_export_catalogue_find(catalogue, component_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    target = &catalogue->targets[catalogue->count];
    (void)memset(target, 0, sizeof(*target));
    status = umi_sdk_copy(target->component_id,
                             sizeof(target->component_id), component_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_copy(target->target_name,
                             sizeof(target->target_name), target_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    target->required = required != 0;
    ++catalogue->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the sdk export catalogue ready operation used by this module and its client
 * applications.
 */
int umi_sdk_export_catalogue_ready(const UmiSdkExportCatalogue *catalogue)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || catalogue->count == 0U ||
        catalogue->count > UMI_SDK_EXPORT_CAPACITY) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!umi_sdk_text(catalogue->targets[index].component_id) ||
            !umi_sdk_text(catalogue->targets[index].target_name)) return 0;
    }
    return 1;
}
