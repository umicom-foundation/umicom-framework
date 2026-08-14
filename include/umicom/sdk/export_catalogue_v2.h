/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/export_catalogue_v2.h
 *
 * PURPOSE:
 *   Catalogue public CMake targets exported by an installed Framework SDK.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SDK_EXPORT_CATALOGUE_V2_H
#define UMICOM_SDK_EXPORT_CATALOGUE_V2_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SDK_V2_ID_CAPACITY 128U
#define UMI_SDK_V2_MAX_EXPORTS 64U

typedef struct UmiSdkExportedTargetV2 {
    char component_id[UMI_SDK_V2_ID_CAPACITY];
    char target_name[UMI_SDK_V2_ID_CAPACITY];
    int required;
} UmiSdkExportedTargetV2;

typedef struct UmiSdkExportCatalogueV2 {
    UmiSdkExportedTargetV2 targets[UMI_SDK_V2_MAX_EXPORTS];
    size_t count;
} UmiSdkExportCatalogueV2;

void umi_sdk_export_catalogue_v2_init(UmiSdkExportCatalogueV2 *catalogue);
UmiStatus umi_sdk_export_catalogue_v2_add(UmiSdkExportCatalogueV2 *catalogue,
                                           const char *component_id,
                                           const char *target_name,
                                           int required);
const UmiSdkExportedTargetV2 *umi_sdk_export_catalogue_v2_find(
    const UmiSdkExportCatalogueV2 *catalogue,
    const char *component_id);
int umi_sdk_export_catalogue_v2_ready(const UmiSdkExportCatalogueV2 *catalogue);

#ifdef __cplusplus
}
#endif
#endif
