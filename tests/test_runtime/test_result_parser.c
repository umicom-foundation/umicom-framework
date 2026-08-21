/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_result_parser.c
 *
 * PURPOSE:
 *   Verify the result parser runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/result_parser.h"
int main(void)
{
    UmiTestRuntimeResultParser value,same;
    uint64_t r;
    umi_test_runtime_result_parser_init(&value,"test-runtime.result_parser");
    assert(umi_test_runtime_result_parser_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_result_parser_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_result_parser_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_result_parser_set_parsed_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_result_parser_set_rejected_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_result_parser_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.parsed_count==13U&&value.rejected_count==21U);
    umi_test_runtime_result_parser_init(&same,"test-runtime.result_parser");
    assert(umi_test_runtime_result_parser_same_identity(&value,&same));
    return 0;
    }
