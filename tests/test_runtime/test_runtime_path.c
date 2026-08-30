/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_runtime_path.c
 *
 * PURPOSE:
 *   Verify the runtime path runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/runtime_path.h"
int main(void)
{
    UmiTestRuntimeRuntimePath value,same;
    uint64_t r;
    umi_test_runtime_runtime_path_init(&value,"test-runtime.runtime_path");
    assert(umi_test_runtime_runtime_path_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_runtime_path_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_path_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_path_set_priority(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_path_set_source_kind(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_path_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.priority==13U&&value.source_kind==21U);
    umi_test_runtime_runtime_path_init(&same,"test-runtime.runtime_path");
    assert(umi_test_runtime_runtime_path_same_identity(&value,&same));
    return 0;
    }
