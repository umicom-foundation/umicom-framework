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

/**
 * Represent the sdk exported target data shared with callers of this public contract.
 */
typedef struct UmiSdkExportedTarget {
    char component_id[UMI_SDK_EXPORT_ID_CAPACITY];
    char target_name[UMI_SDK_EXPORT_ID_CAPACITY];
    int required;
} UmiSdkExportedTarget;

/**
 * Represent the sdk export catalogue data shared with callers of this public contract.
 */
typedef struct UmiSdkExportCatalogue {
    UmiSdkExportedTarget targets[UMI_SDK_EXPORT_CAPACITY];
    size_t count;
} UmiSdkExportCatalogue;

/**
 * Initialise sdk export catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_sdk_export_catalogue_init(UmiSdkExportCatalogue *catalogue);
/**
 * Add sdk export catalogue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_sdk_export_catalogue_add(UmiSdkExportCatalogue *catalogue,
                                           const char *component_id,
                                           const char *target_name,
                                           int required);
/**
 * Find sdk export catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSdkExportedTarget *umi_sdk_export_catalogue_find(
    const UmiSdkExportCatalogue *catalogue,
    const char *component_id);
/**
 * Provide the sdk export catalogue ready operation used by this module and its client
 * applications.
 */
int umi_sdk_export_catalogue_ready(const UmiSdkExportCatalogue *catalogue);

#ifdef __cplusplus
}
#endif
#endif
