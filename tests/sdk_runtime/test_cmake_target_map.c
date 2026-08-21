/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_cmake_target_map.c
 *
 * PURPOSE:
 *   Verify the cmake target map SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/cmake_target_map.h"
int main(void)
{
    UmiSdkRuntimeCmakeTargetMap value,same;
    umi_sdk_runtime_cmake_target_map_init(&value,"sdk-runtime.cmake_target_map");
    assert(umi_sdk_runtime_cmake_target_map_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_cmake_target_map_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_cmake_target_map_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_cmake_target_map_set_target_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_cmake_target_map_set_generation(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_cmake_target_map_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_cmake_target_map_init(&same,"sdk-runtime.cmake_target_map");
    assert(umi_sdk_runtime_cmake_target_map_same_identity(&value,&same));
    return 0;
    }
