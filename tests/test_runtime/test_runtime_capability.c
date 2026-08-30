/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_runtime_capability.c
 *
 * PURPOSE:
 *   Verify the runtime capability runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/runtime_capability.h"
int main(void)
{
    UmiTestRuntimeRuntimeCapability v,s;
    umi_test_runtime_runtime_capability_init(&v,"test-runtime.runtime_capability");
    assert(umi_test_runtime_runtime_capability_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_capability_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_capability_set_required(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_runtime_capability_set_available(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_runtime_capability_init(&s,"test-runtime.runtime_capability");
    assert(umi_test_runtime_runtime_capability_same_identity(&v,&s));
    return 0;
    }
