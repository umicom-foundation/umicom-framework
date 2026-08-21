/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_platform_requirement.c
 *
 * PURPOSE:
 *   Verify the platform requirement runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/platform_requirement.h"
int main(void)
{
    UmiTestRuntimePlatformRequirement v,s;
    umi_test_runtime_platform_requirement_init(&v,"test-runtime.platform_requirement");
    assert(umi_test_runtime_platform_requirement_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_platform_requirement_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_platform_requirement_set_architecture_bits(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_platform_requirement_set_required(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_platform_requirement_init(&s,"test-runtime.platform_requirement");
    assert(umi_test_runtime_platform_requirement_same_identity(&v,&s));
    return 0;
    }
