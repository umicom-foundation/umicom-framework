/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_temporary_directory.c
 *
 * PURPOSE:
 *   Verify the temporary directory runtime contract.
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
#include "umicom/test_runtime/temporary_directory.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeTemporaryDirectory value,same;
    uint64_t r;
    umi_test_runtime_temporary_directory_init(&value,"test-runtime.temporary_directory");
    assert(umi_test_runtime_temporary_directory_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_temporary_directory_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_temporary_directory_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_temporary_directory_set_created(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_temporary_directory_set_cleanup_required(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_temporary_directory_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.created==13U&&value.cleanup_required==21U);
    umi_test_runtime_temporary_directory_init(&same,"test-runtime.temporary_directory");
    assert(umi_test_runtime_temporary_directory_same_identity(&value,&same));
    return 0;
    }
