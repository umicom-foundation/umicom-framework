/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/launch_plan.h
 *
 * PURPOSE:
 *   Convert a reusable project launch profile into a structured run or debug
 *   operation in the shared developer runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_LAUNCH_PLAN_H
#define UMICOM_DEVELOPER_LAUNCH_PLAN_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/runtime.h"
#include "umicom/project/launch_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_LAUNCH_PLAN_API_VERSION 1U

typedef struct UmiDeveloperLaunchPlanRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *launch_profile_id;
    const char *operation_id;
    const char *configuration_id;
    const char *target_id;
    uint32_t timeout_ms;
    uint32_t max_attempts;
    int debug_mode;
} UmiDeveloperLaunchPlanRequest;

typedef struct UmiDeveloperLaunchPlanSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char launch_profile_id[UMI_DEVELOPER_ID_CAPACITY];
    char project_id[UMI_DEVELOPER_ID_CAPACITY];
    char environment_id[UMI_DEVELOPER_ID_CAPACITY];
    UmiDeveloperOperationKind operation_kind;
} UmiDeveloperLaunchPlanSnapshot;

/* debug_mode: -1 follows the profile, 0 forces run, 1 forces debug. */
UmiStatus umi_developer_launch_plan_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan);

#ifdef __cplusplus
}
#endif

#endif
