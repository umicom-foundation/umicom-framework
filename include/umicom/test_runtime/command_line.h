/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/command_line.h
 *
 * PURPOSE:
 *   Retain bounded command-line construction independently of shell parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_COMMAND_LINE
#define UMICOM_TEST_RUNTIME_COMMAND_LINE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeCommandLine {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t argument_count;
    uint64_t byte_count;
    uint64_t revision;
    bool active;
} UmiTestRuntimeCommandLine;
void umi_test_runtime_command_line_init(UmiTestRuntimeCommandLine *value,const char *id);
UmiStatus umi_test_runtime_command_line_validate(const UmiTestRuntimeCommandLine *value);
UmiStatus umi_test_runtime_command_line_set_category(UmiTestRuntimeCommandLine *value,const char *category);
UmiStatus umi_test_runtime_command_line_set_detail(UmiTestRuntimeCommandLine *value,const char *detail);
UmiStatus umi_test_runtime_command_line_set_argument_count(UmiTestRuntimeCommandLine *value,uint64_t number);
UmiStatus umi_test_runtime_command_line_set_byte_count(UmiTestRuntimeCommandLine *value,uint64_t number);
UmiStatus umi_test_runtime_command_line_set_active(UmiTestRuntimeCommandLine *value,bool active);
bool umi_test_runtime_command_line_same_identity(const UmiTestRuntimeCommandLine *left,const UmiTestRuntimeCommandLine *right);
#ifdef __cplusplus
}
#endif
#endif
