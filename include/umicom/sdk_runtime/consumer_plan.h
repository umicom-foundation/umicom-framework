/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/consumer_plan.h
 *
 * PURPOSE:
 *   Plan headers, libraries, runtime files and resources for one application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CONSUMER_PLAN
#define UMICOM_SDK_RUNTIME_CONSUMER_PLAN
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime consumer plan data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeConsumerPlan {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t compile_count;
    uint64_t runtime_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeConsumerPlan;
/**
 * Initialise sdk runtime consumer plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_consumer_plan_init(UmiSdkRuntimeConsumerPlan *value, const char *id);
/**
 * Check that sdk runtime consumer plan satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_consumer_plan_validate(const UmiSdkRuntimeConsumerPlan *value);
/**
 * Provide the sdk runtime consumer plan set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_consumer_plan_set_path(UmiSdkRuntimeConsumerPlan *value, const char *path);
/**
 * Provide the sdk runtime consumer plan set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_consumer_plan_set_detail(UmiSdkRuntimeConsumerPlan *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime consumer plan set compile
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_consumer_plan_set_compile_count(UmiSdkRuntimeConsumerPlan *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime consumer plan set runtime
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_consumer_plan_set_runtime_count(UmiSdkRuntimeConsumerPlan *value, uint64_t number);
/**
 * Provide the sdk runtime consumer plan set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_consumer_plan_set_state(UmiSdkRuntimeConsumerPlan *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime consumer plan same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_consumer_plan_same_identity(const UmiSdkRuntimeConsumerPlan *left, const UmiSdkRuntimeConsumerPlan *right);

/** Publish a complete reviewed value while expected_revision still matches.
 * Copy the live record, edit that copy with the ordinary setters, and retain
 * the revision observed before editing. Both values must validate and have
 * the same ID. A stale review returns INVALID_STATE; exhausted revision space
 * returns CAPACITY_EXCEEDED. All failures leave the live value unchanged.
 * Success copies every proposal field and assigns one new live revision;
 * the proposal's own revision does not control publication. Self-assignment
 * is allowed and also advances once. Other overlapping storage is unsupported.
 * Serialize access on the owner. This value operation allocates nothing and
 * does not perform I/O, authenticate evidence or run the described workflow. */
UmiStatus umi_sdk_runtime_consumer_plan_replace_if_current(UmiSdkRuntimeConsumerPlan *value,
    uint64_t expected_revision, const UmiSdkRuntimeConsumerPlan *proposal);

#ifdef __cplusplus
}
#endif
#endif
