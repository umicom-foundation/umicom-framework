/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/binary_evidence.h
 *
 * PURPOSE:
 *   Retain binary path, size, hash and architecture evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_BINARY_EVIDENCE
#define UMICOM_SDK_RUNTIME_BINARY_EVIDENCE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime binary evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeBinaryEvidence {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t file_size;
    uint64_t checksum;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeBinaryEvidence;
/**
 * Initialise sdk runtime binary evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_binary_evidence_init(UmiSdkRuntimeBinaryEvidence *value, const char *id);
/**
 * Check that sdk runtime binary evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_binary_evidence_validate(const UmiSdkRuntimeBinaryEvidence *value);
/**
 * Provide the sdk runtime binary evidence set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_binary_evidence_set_path(UmiSdkRuntimeBinaryEvidence *value, const char *path);
/**
 * Provide the sdk runtime binary evidence set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_binary_evidence_set_detail(UmiSdkRuntimeBinaryEvidence *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime binary evidence set file without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_binary_evidence_set_file_size(UmiSdkRuntimeBinaryEvidence *value, uint64_t number);
/**
 * Provide the sdk runtime binary evidence set checksum operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_binary_evidence_set_checksum(UmiSdkRuntimeBinaryEvidence *value, uint64_t number);
/**
 * Provide the sdk runtime binary evidence set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_binary_evidence_set_state(UmiSdkRuntimeBinaryEvidence *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime binary evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_binary_evidence_same_identity(const UmiSdkRuntimeBinaryEvidence *left, const UmiSdkRuntimeBinaryEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
