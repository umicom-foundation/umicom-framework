/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_command_line.c
 *
 * PURPOSE:
 *   Verify the command line runtime contract.
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
#include "umicom/test_runtime/command_line.h"
int main(void)
{
    UmiTestRuntimeCommandLine value,same;
    uint64_t r;
    umi_test_runtime_command_line_init(&value,"test-runtime.command_line");
    assert(umi_test_runtime_command_line_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_command_line_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_command_line_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_command_line_set_argument_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_command_line_set_byte_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_command_line_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.argument_count==13U&&value.byte_count==21U);
    umi_test_runtime_command_line_init(&same,"test-runtime.command_line");
    assert(umi_test_runtime_command_line_same_identity(&value,&same));
    return 0;
    }
