/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/cmake_target_map.h
 *
 * PURPOSE:
 *   Map exported CMake target names to installed libraries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_CMAKE_TARGET_MAP
#define UMICOM_SDK_RUNTIME_CMAKE_TARGET_MAP
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeCmakeTargetMap
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t target_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeCmakeTargetMap;
void umi_sdk_runtime_cmake_target_map_init(UmiSdkRuntimeCmakeTargetMap *value,const char *id);
UmiStatus umi_sdk_runtime_cmake_target_map_validate(const UmiSdkRuntimeCmakeTargetMap *value);
UmiStatus umi_sdk_runtime_cmake_target_map_set_path(UmiSdkRuntimeCmakeTargetMap *value,const char *path);
UmiStatus umi_sdk_runtime_cmake_target_map_set_detail(UmiSdkRuntimeCmakeTargetMap *value,const char *detail);
UmiStatus umi_sdk_runtime_cmake_target_map_set_target_count(UmiSdkRuntimeCmakeTargetMap *value,uint64_t number);
UmiStatus umi_sdk_runtime_cmake_target_map_set_generation(UmiSdkRuntimeCmakeTargetMap *value,uint64_t number);
UmiStatus umi_sdk_runtime_cmake_target_map_set_state(UmiSdkRuntimeCmakeTargetMap *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_cmake_target_map_same_identity(const UmiSdkRuntimeCmakeTargetMap *left,const UmiSdkRuntimeCmakeTargetMap *right);
#ifdef __cplusplus
}
#endif
#endif
