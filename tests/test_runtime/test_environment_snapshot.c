/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_environment_snapshot.c
 *
 * PURPOSE:
 *   Verify the environment snapshot runtime contract.
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
#include "umicom/test_runtime/environment_snapshot.h"
int main(void)
{
    UmiTestRuntimeEnvironmentSnapshot value,same;
    uint64_t r;
    umi_test_runtime_environment_snapshot_init(&value,"test-runtime.environment_snapshot");
    assert(umi_test_runtime_environment_snapshot_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_environment_snapshot_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_environment_snapshot_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_environment_snapshot_set_entry_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_environment_snapshot_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_environment_snapshot_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.entry_count==13U&&value.generation==21U);
    umi_test_runtime_environment_snapshot_init(&same,"test-runtime.environment_snapshot");
    assert(umi_test_runtime_environment_snapshot_same_identity(&value,&same));
    return 0;
    }
