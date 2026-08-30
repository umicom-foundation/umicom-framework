/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/export_catalogue.h
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
#ifndef UMICOM_SDK_EXPORT_CATALOGUE_H
#define UMICOM_SDK_EXPORT_CATALOGUE_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_SDK_EXPORT_ID_CAPACITY 128U
#define UMI_SDK_EXPORT_CAPACITY 64U

typedef struct UmiSdkExportedTarget {
    char component_id[UMI_SDK_EXPORT_ID_CAPACITY];
    char target_name[UMI_SDK_EXPORT_ID_CAPACITY];
    int required;
} UmiSdkExportedTarget;

typedef struct UmiSdkExportCatalogue {
    UmiSdkExportedTarget targets[UMI_SDK_EXPORT_CAPACITY];
    size_t count;
} UmiSdkExportCatalogue;

void umi_sdk_export_catalogue_init(UmiSdkExportCatalogue *catalogue);
UmiStatus umi_sdk_export_catalogue_add(UmiSdkExportCatalogue *catalogue,
                                           const char *component_id,
                                           const char *target_name,
                                           int required);
const UmiSdkExportedTarget *umi_sdk_export_catalogue_find(
    const UmiSdkExportCatalogue *catalogue,
    const char *component_id);
int umi_sdk_export_catalogue_ready(const UmiSdkExportCatalogue *catalogue);

#ifdef __cplusplus
}
#endif
#endif
