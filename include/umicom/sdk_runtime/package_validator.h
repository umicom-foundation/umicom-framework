/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_validator.h
 *
 * PURPOSE:
 *   Validate package manifests, paths and dependency closure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_VALIDATOR
#define UMICOM_SDK_RUNTIME_PACKAGE_VALIDATOR
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimePackageValidator {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t checked_count;
    uint64_t failure_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimePackageValidator;
void umi_sdk_runtime_package_validator_init(UmiSdkRuntimePackageValidator *value, const char *id);
UmiStatus umi_sdk_runtime_package_validator_validate(const UmiSdkRuntimePackageValidator *value);
UmiStatus umi_sdk_runtime_package_validator_set_path(UmiSdkRuntimePackageValidator *value, const char *path);
UmiStatus umi_sdk_runtime_package_validator_set_detail(UmiSdkRuntimePackageValidator *value, const char *detail);
UmiStatus umi_sdk_runtime_package_validator_set_checked_count(UmiSdkRuntimePackageValidator *value, uint64_t number);
UmiStatus umi_sdk_runtime_package_validator_set_failure_count(UmiSdkRuntimePackageValidator *value, uint64_t number);
UmiStatus umi_sdk_runtime_package_validator_set_state(UmiSdkRuntimePackageValidator *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_package_validator_same_identity(const UmiSdkRuntimePackageValidator *left, const UmiSdkRuntimePackageValidator *right);
#ifdef __cplusplus
}
#endif
#endif
