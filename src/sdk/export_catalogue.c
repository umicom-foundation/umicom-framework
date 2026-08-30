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

void umi_sdk_export_catalogue_init(UmiSdkExportCatalogue *catalogue)
{
    if (catalogue != NULL) (void)memset(catalogue, 0, sizeof(*catalogue));
}

const UmiSdkExportedTarget *umi_sdk_export_catalogue_find(
    const UmiSdkExportCatalogue *catalogue,
    const char *component_id)
{
    size_t index;
    if (catalogue == NULL || component_id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->targets[index].component_id, component_id) == 0) {
            return &catalogue->targets[index];
        }
    }
    return NULL;
}

UmiStatus umi_sdk_export_catalogue_add(UmiSdkExportCatalogue *catalogue,
                                           const char *component_id,
                                           const char *target_name,
                                           int required)
{
    UmiSdkExportedTarget *target;
    UmiStatus status;
    if (catalogue == NULL || !umi_sdk_text(component_id) ||
        !umi_sdk_text(target_name)) return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->count >= UMI_SDK_EXPORT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_sdk_export_catalogue_find(catalogue, component_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    target = &catalogue->targets[catalogue->count];
    (void)memset(target, 0, sizeof(*target));
    status = umi_sdk_copy(target->component_id,
                             sizeof(target->component_id), component_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_sdk_copy(target->target_name,
                             sizeof(target->target_name), target_name);
    if (status != UMI_STATUS_OK) return status;
    target->required = required != 0;
    ++catalogue->count;
    return UMI_STATUS_OK;
}

int umi_sdk_export_catalogue_ready(const UmiSdkExportCatalogue *catalogue)
{
    size_t index;
    if (catalogue == NULL || catalogue->count == 0U ||
        catalogue->count > UMI_SDK_EXPORT_CAPACITY) return 0;
    for (index = 0U; index < catalogue->count; ++index) {
        if (!umi_sdk_text(catalogue->targets[index].component_id) ||
            !umi_sdk_text(catalogue->targets[index].target_name)) return 0;
    }
    return 1;
}
