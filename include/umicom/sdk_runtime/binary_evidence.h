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
void umi_sdk_runtime_binary_evidence_init(UmiSdkRuntimeBinaryEvidence *value, const char *id);
UmiStatus umi_sdk_runtime_binary_evidence_validate(const UmiSdkRuntimeBinaryEvidence *value);
UmiStatus umi_sdk_runtime_binary_evidence_set_path(UmiSdkRuntimeBinaryEvidence *value, const char *path);
UmiStatus umi_sdk_runtime_binary_evidence_set_detail(UmiSdkRuntimeBinaryEvidence *value, const char *detail);
UmiStatus umi_sdk_runtime_binary_evidence_set_file_size(UmiSdkRuntimeBinaryEvidence *value, uint64_t number);
UmiStatus umi_sdk_runtime_binary_evidence_set_checksum(UmiSdkRuntimeBinaryEvidence *value, uint64_t number);
UmiStatus umi_sdk_runtime_binary_evidence_set_state(UmiSdkRuntimeBinaryEvidence *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_binary_evidence_same_identity(const UmiSdkRuntimeBinaryEvidence *left, const UmiSdkRuntimeBinaryEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
