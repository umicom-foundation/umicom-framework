/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/export_plan_v2.h
 *
 * PURPOSE:
 *   Aggregate installed SDK paths, exported targets and consumer examples.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SDK_EXPORT_PLAN_V2_H
#define UMICOM_SDK_EXPORT_PLAN_V2_H

#include "umicom/sdk/consumer_package_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSdkExportPlanV2 {
    char install_prefix[UMI_SDK_V2_PATH_CAPACITY];
    char include_directory[UMI_SDK_V2_PATH_CAPACITY];
    char library_directory[UMI_SDK_V2_PATH_CAPACITY];
    char examples_directory[UMI_SDK_V2_PATH_CAPACITY];
    UmiSdkConsumerPackageV2 package;
    UmiSdkExportCatalogueV2 catalogue;
} UmiSdkExportPlanV2;

UmiStatus umi_sdk_export_plan_v2_init(UmiSdkExportPlanV2 *plan,
                                       const char *install_prefix,
                                       const char *minimum_version);
UmiStatus umi_sdk_export_plan_v2_add_target(UmiSdkExportPlanV2 *plan,
                                             const char *component_id,
                                             const char *target_name,
                                             int required);
UmiStatus umi_sdk_export_plan_v2_validate(const UmiSdkExportPlanV2 *plan);

#ifdef __cplusplus
}
#endif
#endif
