/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_compiler_runtime.c
 *
 * PURPOSE:
 *   Verify the compiler runtime SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/compiler_runtime.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeCompilerRuntime value,same;
    umi_sdk_runtime_compiler_runtime_init(&value,"sdk-runtime.compiler_runtime");
    assert(umi_sdk_runtime_compiler_runtime_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compiler_runtime_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compiler_runtime_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compiler_runtime_set_dependency_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compiler_runtime_set_missing_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compiler_runtime_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_compiler_runtime_init(&same,"sdk-runtime.compiler_runtime");
    assert(umi_sdk_runtime_compiler_runtime_same_identity(&value,&same));
    return 0;
    }
