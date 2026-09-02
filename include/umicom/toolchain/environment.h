/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/environment.h
 *
 * PURPOSE:
 *   Construct deterministic child-process environments from validated
 *   toolchains and bounded per-task/per-launch overlays without modifying the
 *   parent shell or global PATH.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the environment plan entry data shared with callers of this public contract.
 */
typedef struct UmiEnvironmentPlanEntry {
    char name[UMI_ENVIRONMENT_NAME_CAPACITY];
    char value[UMI_ENVIRONMENT_VALUE_CAPACITY];
} UmiEnvironmentPlanEntry;

/**
 * Represent the environment plan data shared with callers of this public contract.
 */
typedef struct UmiEnvironmentPlan {
    UmiEnvironmentPlanEntry entries[UMI_ENVIRONMENT_PLAN_MAX];
    UmiEnvironmentVariable process_entries[UMI_ENVIRONMENT_PLAN_MAX];
    size_t count;
} UmiEnvironmentPlan;

/**
 * Initialise environment plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_environment_plan_init(UmiEnvironmentPlan *plan);
/**
 * Add environment plan only after its inputs and available capacity have been checked.
 */
UmiStatus umi_environment_plan_add(UmiEnvironmentPlan *plan,
                                   const char *name,
                                   const char *value);
/**
 * Copy environment plan into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_environment_plan_set(UmiEnvironmentPlan *plan,
                                   const char *name,
                                   const char *value);
/**
 * Find environment plan while leaving the underlying catalogue or model owned by this
 * module.
 */
const char *umi_environment_plan_find(const UmiEnvironmentPlan *plan,
                                      const char *name);
/**
 * Provide the environment plan compose operation used by this module and its client
 * applications.
 */
UmiStatus umi_environment_plan_compose(const UmiEnvironmentPlan *base,
                                       const UmiEnvironmentPlan *overlay,
                                       UmiEnvironmentPlan *out_plan);
/**
 * Provide the environment plan append path operation used by this module and its client
 * applications.
 */
UmiStatus umi_environment_plan_append_path(UmiEnvironmentPlan *plan,
                                           const char *directory,
                                           int prepend);
/**
 * Read environment assignment into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_environment_assignment_parse(
    const char *assignment,
    char *out_name,
    size_t name_capacity,
    char *out_value,
    size_t value_capacity);
/**
 * Provide the environment plan set assignment operation used by this module and its client
 * applications.
 */
UmiStatus umi_environment_plan_set_assignment(
    UmiEnvironmentPlan *plan,
    const char *assignment);
/**
 * Check that environment plan satisfies its contract before another service relies on it.
 */
UmiStatus umi_environment_plan_validate(const UmiEnvironmentPlan *plan);
/**
 * Provide the environment plan from toolchain operation used by this module and its client
 * applications.
 */
UmiStatus umi_environment_plan_from_toolchain(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan);
/**
 * Provide the environment plan variables operation used by this module and its client
 * applications.
 */
const UmiEnvironmentVariable *umi_environment_plan_variables(
    UmiEnvironmentPlan *plan);
/**
 * Write environment plan in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_environment_plan_write(const UmiEnvironmentPlan *plan,
                                     const char *path);

#ifdef __cplusplus
}
#endif

#endif
