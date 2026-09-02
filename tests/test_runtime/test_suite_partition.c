/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_suite_partition.c
 *
 * PURPOSE:
 *   Verify the suite partition runtime contract.
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
#include "umicom/test_runtime/suite_partition.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeSuitePartition value,same;
    uint64_t r;
    umi_test_runtime_suite_partition_init(&value,"test-runtime.suite_partition");
    assert(umi_test_runtime_suite_partition_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_suite_partition_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_suite_partition_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_suite_partition_set_partition_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_suite_partition_set_member_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_suite_partition_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.partition_count==13U&&value.member_count==21U);
    umi_test_runtime_suite_partition_init(&same,"test-runtime.suite_partition");
    assert(umi_test_runtime_suite_partition_same_identity(&value,&same));
    return 0;
    }
