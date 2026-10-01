/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/loader_evidence.h
 *
 * PURPOSE:
 *   Retain successful and failed runtime-loader resolution evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_LOADER_EVIDENCE
#define UMICOM_SDK_RUNTIME_LOADER_EVIDENCE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime loader evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeLoaderEvidence {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t attempt_count;
    uint64_t resolved_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeLoaderEvidence;
/**
 * Initialise sdk runtime loader evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_loader_evidence_init(UmiSdkRuntimeLoaderEvidence *value, const char *id);
/**
 * Check that sdk runtime loader evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_loader_evidence_validate(const UmiSdkRuntimeLoaderEvidence *value);
/**
 * Provide the sdk runtime loader evidence set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_evidence_set_path(UmiSdkRuntimeLoaderEvidence *value, const char *path);
/**
 * Provide the sdk runtime loader evidence set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_evidence_set_detail(UmiSdkRuntimeLoaderEvidence *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime loader evidence set attempt
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_loader_evidence_set_attempt_count(UmiSdkRuntimeLoaderEvidence *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime loader evidence set resolved
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_loader_evidence_set_resolved_count(UmiSdkRuntimeLoaderEvidence *value, uint64_t number);
/**
 * Provide the sdk runtime loader evidence set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_loader_evidence_set_state(UmiSdkRuntimeLoaderEvidence *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime loader evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_loader_evidence_same_identity(const UmiSdkRuntimeLoaderEvidence *left, const UmiSdkRuntimeLoaderEvidence *right);

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
UmiStatus umi_sdk_runtime_loader_evidence_replace_if_current(UmiSdkRuntimeLoaderEvidence *value,
    uint64_t expected_revision, const UmiSdkRuntimeLoaderEvidence *proposal);

#ifdef __cplusplus
}
#endif
#endif
