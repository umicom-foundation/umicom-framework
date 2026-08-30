/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_network_requirement.c
 *
 * PURPOSE:
 *   Verify the network requirement runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/network_requirement.h"
int main(void)
{
    UmiTestRuntimeNetworkRequirement v,s;
    umi_test_runtime_network_requirement_init(&v,"test-runtime.network_requirement");
    assert(umi_test_runtime_network_requirement_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_network_requirement_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_network_requirement_set_required(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_network_requirement_set_isolated(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_network_requirement_init(&s,"test-runtime.network_requirement");
    assert(umi_test_runtime_network_requirement_same_identity(&v,&s));
    return 0;
    }
