/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/loader_evidence.h
 *
 * PURPOSE:
 *   Retain successful and failed runtime-loader resolution evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_LOADER_EVIDENCE
#define UMICOM_SDK_RUNTIME_LOADER_EVIDENCE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_sdk_runtime_loader_evidence_init(UmiSdkRuntimeLoaderEvidence *value, const char *id);
UmiStatus umi_sdk_runtime_loader_evidence_validate(const UmiSdkRuntimeLoaderEvidence *value);
UmiStatus umi_sdk_runtime_loader_evidence_set_path(UmiSdkRuntimeLoaderEvidence *value, const char *path);
UmiStatus umi_sdk_runtime_loader_evidence_set_detail(UmiSdkRuntimeLoaderEvidence *value, const char *detail);
UmiStatus umi_sdk_runtime_loader_evidence_set_attempt_count(UmiSdkRuntimeLoaderEvidence *value, uint64_t number);
UmiStatus umi_sdk_runtime_loader_evidence_set_resolved_count(UmiSdkRuntimeLoaderEvidence *value, uint64_t number);
UmiStatus umi_sdk_runtime_loader_evidence_set_state(UmiSdkRuntimeLoaderEvidence *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_loader_evidence_same_identity(const UmiSdkRuntimeLoaderEvidence *left, const UmiSdkRuntimeLoaderEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
