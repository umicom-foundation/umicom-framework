/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_evidence.h
 *
 * PURPOSE:
 *   Aggregate validation evidence for one SDK/runtime package.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_EVIDENCE
#define UMICOM_SDK_RUNTIME_PACKAGE_EVIDENCE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime package evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimePackageEvidence {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t passed_count;
    uint64_t failed_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimePackageEvidence;
/**
 * Initialise sdk runtime package evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_package_evidence_init(UmiSdkRuntimePackageEvidence *value, const char *id);
/**
 * Check that sdk runtime package evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_package_evidence_validate(const UmiSdkRuntimePackageEvidence *value);
/**
 * Provide the sdk runtime package evidence set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_evidence_set_path(UmiSdkRuntimePackageEvidence *value, const char *path);
/**
 * Provide the sdk runtime package evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_package_evidence_set_detail(UmiSdkRuntimePackageEvidence *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime package evidence set passed
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_evidence_set_passed_count(UmiSdkRuntimePackageEvidence *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime package evidence set failed
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_evidence_set_failed_count(UmiSdkRuntimePackageEvidence *value, uint64_t number);
/**
 * Provide the sdk runtime package evidence set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_evidence_set_state(UmiSdkRuntimePackageEvidence *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime package evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_package_evidence_same_identity(const UmiSdkRuntimePackageEvidence *left, const UmiSdkRuntimePackageEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
