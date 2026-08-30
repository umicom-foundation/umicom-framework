/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_environment_entry.c
 *
 * PURPOSE:
 *   Verify the environment entry runtime contract.
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
#include "umicom/test_runtime/environment_entry.h"
int main(void)
{
    UmiTestRuntimeEnvironmentEntry value,same;
    uint64_t r;
    umi_test_runtime_environment_entry_init(&value,"test-runtime.environment_entry");
    assert(umi_test_runtime_environment_entry_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_environment_entry_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_environment_entry_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_environment_entry_set_required(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_environment_entry_set_redacted(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_environment_entry_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.required==13U&&value.redacted==21U);
    umi_test_runtime_environment_entry_init(&same,"test-runtime.environment_entry");
    assert(umi_test_runtime_environment_entry_same_identity(&value,&same));
    return 0;
    }
