/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/task_plan.h
 *
 * PURPOSE:
 *   Convert a reusable project task into a structured developer operation and
 *   submit it to the shared developer runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Tasks remain project metadata.  This module only translates an enabled task
 * into the common operation pipeline, so Studio, a CLI, a remote worker and a
 * future Umicom application all observe the same execution lifecycle.
 */
#ifndef UMICOM_DEVELOPER_TASK_PLAN_H
#define UMICOM_DEVELOPER_TASK_PLAN_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/runtime.h"
#include "umicom/project/task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_TASK_PLAN_API_VERSION 1U

typedef struct UmiDeveloperTaskPlanRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *task_id;
    const char *operation_id;
    const char *configuration_id;
    const char *target_id;
    uint32_t timeout_ms;
    uint32_t max_attempts;
} UmiDeveloperTaskPlanRequest;

typedef struct UmiDeveloperTaskPlanSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char task_id[UMI_DEVELOPER_ID_CAPACITY];
    char project_id[UMI_DEVELOPER_ID_CAPACITY];
    char group[UMI_DEVELOPER_ID_CAPACITY];
    int background;
} UmiDeveloperTaskPlanSnapshot;

UmiStatus umi_developer_task_plan_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperTaskPlanRequest *request,
    UmiDeveloperTaskPlanSnapshot *out_plan);

#ifdef __cplusplus
}
#endif

#endif
