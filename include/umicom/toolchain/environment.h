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
 * Add one owned entry to a valid plan. Reject duplicate names, malformed text
 * and capacity overflow without changing the plan. Names remain case-sensitive.
 */
UmiStatus umi_environment_plan_add(UmiEnvironmentPlan *plan,
                                   const char *name,
                                   const char *value);
/**
 * Set one owned entry after validating the plan and candidate text. A value
 * borrowed from this same plan is supported; refusal preserves the old state.
 */
UmiStatus umi_environment_plan_set(UmiEnvironmentPlan *plan,
                                   const char *name,
                                   const char *value);
/**
 * Borrow a value from a valid plan. NULL means absent or invalid; call the
 * validator separately when the reason is needed. Mutation can invalidate it.
 */
const char *umi_environment_plan_find(const UmiEnvironmentPlan *plan,
                                      const char *name);
/**
 * Compose optional base and overlay into an independently staged candidate.
 * Overlay values replace matching base names. Either input may be out_plan;
 * failure leaves out_plan unchanged. Heap allocation failure is reported.
 * Successful process-entry pointers always refer to out_plan storage.
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
 * Build an owned plan from a compatible profile. Missing optional tools are
 * allowed; malformed validated tool paths and preparation errors are reported.
 * Failure preserves out_plan. An absent inherited PATH adds no empty segment.
 */
UmiStatus umi_environment_plan_from_toolchain(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *out_plan);
/**
 * Validate and rebind borrowed process entries to this plan. Invalid plans
 * return NULL without rebinding. Keep the plan alive and stable during use.
 */
const UmiEnvironmentVariable *umi_environment_plan_variables(
    UmiEnvironmentPlan *plan);
/**
 * Validate and format the complete plan before opening its text destination.
 * Admission/formatting refusal performs no write. File-write failures retain
 * the filesystem service semantics; this is not an atomic filesystem snapshot.
 */
UmiStatus umi_environment_plan_write(const UmiEnvironmentPlan *plan,
                                     const char *path);

#ifdef __cplusplus
}
#endif

#endif
