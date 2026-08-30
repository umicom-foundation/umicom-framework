/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_process_identity.c
 *
 * PURPOSE:
 *   Verify the process identity runtime contract.
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
#include "umicom/test_runtime/process_identity.h"
int main(void)
{
    UmiTestRuntimeProcessIdentity value,same;
    uint64_t r;
    umi_test_runtime_process_identity_init(&value,"test-runtime.process_identity");
    assert(umi_test_runtime_process_identity_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_process_identity_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_process_identity_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_process_identity_set_process_id(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_process_identity_set_parent_process_id(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_process_identity_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.process_id==13U&&value.parent_process_id==21U);
    umi_test_runtime_process_identity_init(&same,"test-runtime.process_identity");
    assert(umi_test_runtime_process_identity_same_identity(&value,&same));
    return 0;
    }
