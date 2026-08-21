/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/result_parser.h
 *
 * PURPOSE:
 *   Retain result-parser counters and malformed-evidence diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RESULT_PARSER
#define UMICOM_TEST_RUNTIME_RESULT_PARSER
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_test_runtime_result_parser_init(UmiTestRuntimeResultParser *value,const char *id);
UmiStatus umi_test_runtime_result_parser_validate(const UmiTestRuntimeResultParser *value);
UmiStatus umi_test_runtime_result_parser_set_category(UmiTestRuntimeResultParser *value,const char *category);
UmiStatus umi_test_runtime_result_parser_set_detail(UmiTestRuntimeResultParser *value,const char *detail);
UmiStatus umi_test_runtime_result_parser_set_parsed_count(UmiTestRuntimeResultParser *value,uint64_t number);
UmiStatus umi_test_runtime_result_parser_set_rejected_count(UmiTestRuntimeResultParser *value,uint64_t number);
UmiStatus umi_test_runtime_result_parser_set_active(UmiTestRuntimeResultParser *value,bool active);
bool umi_test_runtime_result_parser_same_identity(const UmiTestRuntimeResultParser *left,const UmiTestRuntimeResultParser *right);
#ifdef __cplusplus
}
#endif
#endif
