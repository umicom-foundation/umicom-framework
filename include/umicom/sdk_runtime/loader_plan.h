/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/loader_plan.h
 *
 * PURPOSE:
 *   Describe runtime load order without exposing platform loader handles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_LOADER_PLAN
#define UMICOM_SDK_RUNTIME_LOADER_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime loader plan data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeLoaderPlan {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeLoaderPlan;
/**
 * Initialise sdk runtime loader plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_loader_plan_init(UmiSdkRuntimeLoaderPlan *value, const char *id);
/**
 * Check that sdk runtime loader plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_loader_plan_validate(const UmiSdkRuntimeLoaderPlan *value);
/**
 * Provide the sdk runtime loader plan set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_plan_set_path(UmiSdkRuntimeLoaderPlan *value, const char *path);
/**
 * Provide the sdk runtime loader plan set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_plan_set_detail(UmiSdkRuntimeLoaderPlan *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime loader plan set entry without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_loader_plan_set_entry_count(UmiSdkRuntimeLoaderPlan *value, uint64_t number);
/**
 * Provide the sdk runtime loader plan set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_plan_set_generation(UmiSdkRuntimeLoaderPlan *value, uint64_t number);
/**
 * Provide the sdk runtime loader plan set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_plan_set_state(UmiSdkRuntimeLoaderPlan *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime loader plan same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_loader_plan_same_identity(const UmiSdkRuntimeLoaderPlan *left, const UmiSdkRuntimeLoaderPlan *right);
#ifdef __cplusplus
}
#endif
#endif
