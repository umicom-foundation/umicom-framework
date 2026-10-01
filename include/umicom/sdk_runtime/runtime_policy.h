/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_policy.h
 *
 * PURPOSE:
 *   Define source-build, installed-SDK and self-contained runtime policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_POLICY
#define UMICOM_SDK_RUNTIME_RUNTIME_POLICY
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime runtime policy data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeRuntimePolicy {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t allow_source;
    uint64_t require_installed;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeRuntimePolicy;
/**
 * Initialise sdk runtime runtime policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_runtime_policy_init(UmiSdkRuntimeRuntimePolicy *value, const char *id);
/**
 * Check that sdk runtime runtime policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_runtime_policy_validate(const UmiSdkRuntimeRuntimePolicy *value);
/**
 * Provide the sdk runtime runtime policy set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_path(UmiSdkRuntimeRuntimePolicy *value, const char *path);
/**
 * Provide the sdk runtime runtime policy set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_detail(UmiSdkRuntimeRuntimePolicy *value, const char *detail);
/**
 * Provide the sdk runtime runtime policy set allow source operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_allow_source(UmiSdkRuntimeRuntimePolicy *value, uint64_t number);
/**
 * Provide the sdk runtime runtime policy set require installed operation used by this
 * module and its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_require_installed(UmiSdkRuntimeRuntimePolicy *value, uint64_t number);
/**
 * Provide the sdk runtime runtime policy set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_state(UmiSdkRuntimeRuntimePolicy *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime runtime policy same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_runtime_policy_same_identity(const UmiSdkRuntimeRuntimePolicy *left, const UmiSdkRuntimeRuntimePolicy *right);

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
UmiStatus umi_sdk_runtime_runtime_policy_replace_if_current(UmiSdkRuntimeRuntimePolicy *value,
    uint64_t expected_revision, const UmiSdkRuntimeRuntimePolicy *proposal);

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
UmiStatus umi_sdk_runtime_runtime_policy_init_checked(UmiSdkRuntimeRuntimePolicy *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
