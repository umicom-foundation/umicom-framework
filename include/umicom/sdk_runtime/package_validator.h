/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_validator.h
 *
 * PURPOSE:
 *   Validate package manifests, paths and dependency closure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_VALIDATOR
#define UMICOM_SDK_RUNTIME_PACKAGE_VALIDATOR
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime package validator data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise sdk runtime package validator from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_package_validator_init(UmiSdkRuntimePackageValidator *value, const char *id);
/**
 * Check that sdk runtime package validator satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_package_validator_validate(const UmiSdkRuntimePackageValidator *value);
/**
 * Provide the sdk runtime package validator set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_validator_set_path(UmiSdkRuntimePackageValidator *value, const char *path);
/**
 * Provide the sdk runtime package validator set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_package_validator_set_detail(UmiSdkRuntimePackageValidator *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime package validator set checked
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_validator_set_checked_count(UmiSdkRuntimePackageValidator *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime package validator set failure
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_validator_set_failure_count(UmiSdkRuntimePackageValidator *value, uint64_t number);
/**
 * Provide the sdk runtime package validator set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_package_validator_set_state(UmiSdkRuntimePackageValidator *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime package validator same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_package_validator_same_identity(const UmiSdkRuntimePackageValidator *left, const UmiSdkRuntimePackageValidator *right);
#ifdef __cplusplus
}
#endif
#endif
