/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/environment.h
 *
 * PURPOSE:
 *   Construct deterministic child-process environments from validated
 *   toolchains and bounded per-task/per-launch overlays without modifying the
 *   parent shell or global PATH.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_ENVIRONMENT_H
#define UMICOM_TOOLCHAIN_ENVIRONMENT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_ENVIRONMENT_PLAN_MAX 24U
#define UMI_ENVIRONMENT_NAME_CAPACITY 128U
#define UMI_ENVIRONMENT_VALUE_CAPACITY 8192U

typedef struct UmiEnvironmentPlanEntry {
    char name[UMI_ENVIRONMENT_NAME_CAPACITY];
    char value[UMI_ENVIRONMENT_VALUE_CAPACITY];
} UmiEnvironmentPlanEntry;

typedef struct UmiEnvironmentPlan {
    UmiEnvironmentPlanEntry entries[UMI_ENVIRONMENT_PLAN_MAX];
    UmiEnvironmentVariable process_entries[UMI_ENVIRONMENT_PLAN_MAX];
    size_t count;
} UmiEnvironmentPlan;

void umi_environment_plan_init(UmiEnvironmentPlan *plan);
UmiStatus umi_environment_plan_add(UmiEnvironmentPlan *plan,
                                   const char *name,
                                   const char *value);
UmiStatus umi_environment_plan_set(UmiEnvironmentPlan *plan,
                                   const char *name,
                                   const char *value);
const char *umi_environment_plan_find(const UmiEnvironmentPlan *plan,
                                      const char *name);
UmiStatus umi_environment_plan_compose(const UmiEnvironmentPlan *base,
                                       const UmiEnvironmentPlan *overlay,
                                       UmiEnvironmentPlan *out_plan);
UmiStatus umi_environment_plan_append_path(UmiEnvironmentPlan *plan,
                                           const char *directory,
                                           int prepend);
UmiStatus umi_environment_assignment_parse(
    const char *assignment,
    char *out_name,
    size_t name_capacity,
    char *out_value,
    size_t value_capacity);
UmiStatus umi_environment_plan_set_assignment(
    UmiEnvironmentPlan *plan,
    const char *assignment);
UmiStatus umi_environment_plan_validate(const UmiEnvironmentPlan *plan);
UmiStatus umi_environment_plan_from_toolchain(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan);
const UmiEnvironmentVariable *umi_environment_plan_variables(
    UmiEnvironmentPlan *plan);
UmiStatus umi_environment_plan_write(const UmiEnvironmentPlan *plan,
                                     const char *path);

#ifdef __cplusplus
}
#endif

#endif
