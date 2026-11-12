/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/launch_plan.h
 *
 * PURPOSE:
 *   Convert a reusable project launch profile into a structured run or debug
 *   operation in the shared developer runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the developer launch plan request data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the developer launch plan snapshot data shared with callers of this public
 * contract.
 */
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
/* Translate one existing project record into an owned operation value without
 * queueing it, adding journal events or executing a process. out_operation is
 * required; out_plan is optional. Both outputs remain unchanged on failure.
 * Inputs and the single-owner runtime must stay stable during this call;
 * output objects must not overlap. Use submit for one immediate queue entry,
 * or build several values and publish them through runtime_submit_chain. */
UmiStatus umi_developer_launch_plan_build(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperOperationSnapshot *out_operation,
    UmiDeveloperLaunchPlanSnapshot *out_plan);

UmiStatus umi_developer_launch_plan_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperLaunchPlanRequest *request,
    UmiDeveloperLaunchPlanSnapshot *out_plan);


/* Compare a caller's previously captured profile with the current registry
 * record of the same id. Capture with registry_find/at before review: upsert
 * may normalize fields and assigns the live revision. NULL/0 workspace only
 * measures; short storage reports required_workspace and NOT_COMPARED.
 * Missing or invalid profiles leave the output unchanged. No queue, journal,
 * registry or process is changed. Keep all inputs, scratch and output outside
 * the runtime's storage and obey value_archive.h alias rules. Serialize the
 * runtime across this call; equality is an observation, not a lock or approval
 * for a later submission. Recheck immediately before acting on a review. */
UmiStatus umi_developer_launch_plan_compare_profile(
    UmiDeveloperRuntime *runtime,
    const UmiProjectLaunchProfileSnapshot *expected,
    void *workspace, size_t capacity,
    UmiValueArchiveComparison *out_comparison);


/** Queue a launch only if the captured profile still matches the live saved
 * fields and revision. Capture with registry_find/at; measure scratch using
 * compare_profile before calling. Unlike comparison, this requires non-NULL
 * scratch and never treats a measurement as permission to queue.
 *
 * request must use the current size/version and the same launch_profile_id
 * as expected. Request overrides (debug mode, target, configuration, timeout
 * and attempts) are explicit caller choices, not covered by the profile
 * comparison. Callers must separately present/admit those choices.
 *
 * Changed values return INVALID_STATE; a removed profile returns NOT_FOUND;
 * short scratch returns CAPACITY_EXCEEDED. Every refusal retains out_plan,
 * the queue, journal and runtime revision. Scratch may change after admission.
 * Success queues one owned operation; it does not run a process. Later profile
 * edits do not change that queued operation. Existing submission remains
 * available for intentional live lookup.
 *
 * Serialize the runtime across this entire call and do not mutate it from
 * notification callbacks. All inputs, scratch and optional out_plan must stay
 * valid and outside runtime storage. Scratch/output must not overlap inputs
 * or each other. This is an owner-thread check, not a lock, filesystem identity
 * check, permission grant or approval of a changed executable on disk.
 */
UmiStatus umi_developer_launch_plan_submit_reviewed(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperLaunchPlanRequest *request,
    const UmiProjectLaunchProfileSnapshot *expected,
    void *workspace, size_t capacity,
    UmiDeveloperLaunchPlanSnapshot *out_plan);

#ifdef __cplusplus
}
#endif

#endif
