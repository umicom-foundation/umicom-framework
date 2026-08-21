/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_evidence.h
 *
 * PURPOSE:
 *   Aggregate validation evidence for one SDK/runtime package.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_EVIDENCE
#define UMICOM_SDK_RUNTIME_PACKAGE_EVIDENCE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_sdk_runtime_package_evidence_init(UmiSdkRuntimePackageEvidence *value, const char *id);
UmiStatus umi_sdk_runtime_package_evidence_validate(const UmiSdkRuntimePackageEvidence *value);
UmiStatus umi_sdk_runtime_package_evidence_set_path(UmiSdkRuntimePackageEvidence *value, const char *path);
UmiStatus umi_sdk_runtime_package_evidence_set_detail(UmiSdkRuntimePackageEvidence *value, const char *detail);
UmiStatus umi_sdk_runtime_package_evidence_set_passed_count(UmiSdkRuntimePackageEvidence *value, uint64_t number);
UmiStatus umi_sdk_runtime_package_evidence_set_failed_count(UmiSdkRuntimePackageEvidence *value, uint64_t number);
UmiStatus umi_sdk_runtime_package_evidence_set_state(UmiSdkRuntimePackageEvidence *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_package_evidence_same_identity(const UmiSdkRuntimePackageEvidence *left, const UmiSdkRuntimePackageEvidence *right);
#ifdef __cplusplus
}
#endif
#endif
