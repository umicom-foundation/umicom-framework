/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/cmake_target_map.h
 *
 * PURPOSE:
 *   Map exported CMake target names to installed libraries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CMAKE_TARGET_MAP
#define UMICOM_SDK_RUNTIME_CMAKE_TARGET_MAP
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime cmake target map data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeCmakeTargetMap
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t target_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeCmakeTargetMap;
/**
 * Initialise sdk runtime cmake target map from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_cmake_target_map_init(UmiSdkRuntimeCmakeTargetMap *value,const char *id);
/**
 * Check that sdk runtime cmake target map satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_cmake_target_map_validate(const UmiSdkRuntimeCmakeTargetMap *value);
/**
 * Provide the sdk runtime cmake target map set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_cmake_target_map_set_path(UmiSdkRuntimeCmakeTargetMap *value,const char *path);
/**
 * Provide the sdk runtime cmake target map set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_cmake_target_map_set_detail(UmiSdkRuntimeCmakeTargetMap *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime cmake target map set target
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_cmake_target_map_set_target_count(UmiSdkRuntimeCmakeTargetMap *value,uint64_t number);
/**
 * Provide the sdk runtime cmake target map set generation operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_cmake_target_map_set_generation(UmiSdkRuntimeCmakeTargetMap *value,uint64_t number);
/**
 * Provide the sdk runtime cmake target map set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_cmake_target_map_set_state(UmiSdkRuntimeCmakeTargetMap *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime cmake target map same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_cmake_target_map_same_identity(const UmiSdkRuntimeCmakeTargetMap *left,const UmiSdkRuntimeCmakeTargetMap *right);
#ifdef __cplusplus
}
#endif
#endif
