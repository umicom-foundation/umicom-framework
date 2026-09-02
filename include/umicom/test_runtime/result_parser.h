/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/result_parser.h
 *
 * PURPOSE:
 *   Retain result-parser counters and malformed-evidence diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESULT_PARSER
#define UMICOM_TEST_RUNTIME_RESULT_PARSER
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime result parser data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeResultParser {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t parsed_count;
    uint64_t rejected_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeResultParser;
/**
 * Initialise test runtime result parser from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_result_parser_init(UmiTestRuntimeResultParser *value,const char *id);
/**
 * Check that test runtime result parser satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_result_parser_validate(const UmiTestRuntimeResultParser *value);
/**
 * Provide the test runtime result parser set category operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_result_parser_set_category(UmiTestRuntimeResultParser *value,const char *category);
/**
 * Provide the test runtime result parser set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_result_parser_set_detail(UmiTestRuntimeResultParser *value,const char *detail);
/**
 * Return the number of records represented by test runtime result parser set parsed
 * without changing their state.
 */
UmiStatus umi_test_runtime_result_parser_set_parsed_count(UmiTestRuntimeResultParser *value,uint64_t number);
/**
 * Return the number of records represented by test runtime result parser set rejected
 * without changing their state.
 */
UmiStatus umi_test_runtime_result_parser_set_rejected_count(UmiTestRuntimeResultParser *value,uint64_t number);
/**
 * Provide the test runtime result parser set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_result_parser_set_active(UmiTestRuntimeResultParser *value,bool active);
/**
 * Provide the test runtime result parser same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_result_parser_same_identity(const UmiTestRuntimeResultParser *left,const UmiTestRuntimeResultParser *right);
#ifdef __cplusplus
}
#endif
#endif
