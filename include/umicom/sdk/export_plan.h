/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/export_plan.h
 *
 * PURPOSE:
 *   Aggregate installed SDK paths, exported targets and consumer examples.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SDK_EXPORT_PLAN_H
#define UMICOM_SDK_EXPORT_PLAN_H

#include "umicom/sdk/consumer_package.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSdkExportPlan {
    char install_prefix[UMI_SDK_PACKAGE_PATH_CAPACITY];
    char include_directory[UMI_SDK_PACKAGE_PATH_CAPACITY];
    char library_directory[UMI_SDK_PACKAGE_PATH_CAPACITY];
    char examples_directory[UMI_SDK_PACKAGE_PATH_CAPACITY];
    UmiSdkConsumerPackage package;
    UmiSdkExportCatalogue catalogue;
} UmiSdkExportPlan;

UmiStatus umi_sdk_export_plan_init(UmiSdkExportPlan *plan,
                                       const char *install_prefix,
                                       const char *minimum_version);
UmiStatus umi_sdk_export_plan_add_target(UmiSdkExportPlan *plan,
                                             const char *component_id,
                                             const char *target_name,
                                             int required);
UmiStatus umi_sdk_export_plan_validate(const UmiSdkExportPlan *plan);

#ifdef __cplusplus
}
#endif
#endif
