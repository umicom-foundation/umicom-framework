/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/cmake_package.h
 *
 * PURPOSE:
 *   Describe Config.cmake, targets file and package search evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CMAKE_PACKAGE
#define UMICOM_SDK_RUNTIME_CMAKE_PACKAGE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime cmake package data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeCmakePackage {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t target_count;
    uint64_t found;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeCmakePackage;
/**
 * Initialise sdk runtime cmake package from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_cmake_package_init(UmiSdkRuntimeCmakePackage *value, const char *id);
/**
 * Check that sdk runtime cmake package satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_cmake_package_validate(const UmiSdkRuntimeCmakePackage *value);
/**
 * Provide the sdk runtime cmake package set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_cmake_package_set_path(UmiSdkRuntimeCmakePackage *value, const char *path);
/**
 * Provide the sdk runtime cmake package set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_cmake_package_set_detail(UmiSdkRuntimeCmakePackage *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime cmake package set target without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_cmake_package_set_target_count(UmiSdkRuntimeCmakePackage *value, uint64_t number);
/**
 * Provide the sdk runtime cmake package set found operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_cmake_package_set_found(UmiSdkRuntimeCmakePackage *value, uint64_t number);
/**
 * Provide the sdk runtime cmake package set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_cmake_package_set_state(UmiSdkRuntimeCmakePackage *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime cmake package same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_cmake_package_same_identity(const UmiSdkRuntimeCmakePackage *left, const UmiSdkRuntimeCmakePackage *right);
#ifdef __cplusplus
}
#endif
#endif
