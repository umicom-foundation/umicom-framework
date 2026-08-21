/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_runtime_manifest.c
 *
 * PURPOSE:
 *   Verify the runtime manifest runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/runtime_manifest.h"
int main(void)
{
    UmiTestRuntimeRuntimeManifest value,same;
    uint64_t r;
    umi_test_runtime_runtime_manifest_init(&value,"test-runtime.runtime_manifest");
    assert(umi_test_runtime_runtime_manifest_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_runtime_manifest_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_manifest_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_manifest_set_file_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_manifest_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_manifest_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.file_count==13U&&value.generation==21U);
    umi_test_runtime_runtime_manifest_init(&same,"test-runtime.runtime_manifest");
    assert(umi_test_runtime_runtime_manifest_same_identity(&value,&same));
    return 0;
    }
