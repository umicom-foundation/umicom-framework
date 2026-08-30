/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/dependency_evidence.h
 *
 * PURPOSE:
 *   Retain resolved dependency path and provenance evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_DEPENDENCY_EVIDENCE
#define UMICOM_SDK_RUNTIME_DEPENDENCY_EVIDENCE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeDependencyEvidence {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t resolved;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeDependencyEvidence;
void umi_sdk_runtime_dependency_evidence_init(UmiSdkRuntimeDependencyEvidence *value, const char *id);
UmiStatus umi_sdk_runtime_dependency_evidence_validate(const UmiSdkRuntimeDependencyEvidence *value);
UmiStatus umi_sdk_runtime_dependency_evidence_set_path(UmiSdkRuntimeDependencyEvidence *value, const char *path);
UmiStatus umi_sdk_runtime_dependency_evidence_set_detail(UmiSdkRuntimeDependencyEvidence *value, const char *detail);
UmiStatus umi_sdk_runtime_dependency_evidence_set_required(UmiSdkRuntimeDependencyEvidence *value, uint64_t number);
UmiStatus umi_sdk_runtime_dependency_evidence_set_resolved(UmiSdkRuntimeDependencyEvidence *value, uint64_t number);
UmiStatus umi_sdk_runtime_dependency_evidence_set_state(UmiSdkRuntimeDependencyEvidence *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_dependency_evidence_same_identity(const UmiSdkRuntimeDependencyEvidence *left, const UmiSdkRuntimeDependencyEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
