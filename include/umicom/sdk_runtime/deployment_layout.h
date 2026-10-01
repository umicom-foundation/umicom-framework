/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/deployment_layout.h
 *
 * PURPOSE:
 *   Describe relocatable runtime deployment directories and resource roots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_DEPLOYMENT_LAYOUT
#define UMICOM_SDK_RUNTIME_DEPLOYMENT_LAYOUT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime deployment layout data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeDeploymentLayout {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t directory_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeDeploymentLayout;
/**
 * Initialise sdk runtime deployment layout from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_deployment_layout_init(UmiSdkRuntimeDeploymentLayout *value, const char *id);
/**
 * Check that sdk runtime deployment layout satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_deployment_layout_validate(const UmiSdkRuntimeDeploymentLayout *value);
/**
 * Provide the sdk runtime deployment layout set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_path(UmiSdkRuntimeDeploymentLayout *value, const char *path);
/**
 * Provide the sdk runtime deployment layout set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_detail(UmiSdkRuntimeDeploymentLayout *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime deployment layout set directory
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_directory_count(UmiSdkRuntimeDeploymentLayout *value, uint64_t number);
/**
 * Provide the sdk runtime deployment layout set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_generation(UmiSdkRuntimeDeploymentLayout *value, uint64_t number);
/**
 * Provide the sdk runtime deployment layout set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_deployment_layout_set_state(UmiSdkRuntimeDeploymentLayout *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime deployment layout same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_deployment_layout_same_identity(const UmiSdkRuntimeDeploymentLayout *left, const UmiSdkRuntimeDeploymentLayout *right);

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
UmiStatus umi_sdk_runtime_deployment_layout_replace_if_current(UmiSdkRuntimeDeploymentLayout *value,
    uint64_t expected_revision, const UmiSdkRuntimeDeploymentLayout *proposal);

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_sdk_runtime_deployment_layout_init_checked(UmiSdkRuntimeDeploymentLayout *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
