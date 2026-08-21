/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/command.h
 *
 * PURPOSE:
 *   Describe typed Master Controller requests for test-runtime operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_COMMAND
#define UMICOM_TEST_RUNTIME_COMMAND

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeCommand {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t kind;
    uint64_t sequence;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeCommand;

void umi_test_runtime_command_init(UmiTestRuntimeCommand *value, const char *id);
UmiStatus umi_test_runtime_command_validate(const UmiTestRuntimeCommand *value);
UmiStatus umi_test_runtime_command_set_name(UmiTestRuntimeCommand *value, const char *name);
UmiStatus umi_test_runtime_command_set_detail(UmiTestRuntimeCommand *value, const char *detail);
UmiStatus umi_test_runtime_command_set_kind(UmiTestRuntimeCommand *value, uint64_t number);
UmiStatus umi_test_runtime_command_set_sequence(UmiTestRuntimeCommand *value, uint64_t number);
UmiStatus umi_test_runtime_command_touch(UmiTestRuntimeCommand *value, uint64_t updated_at_ms);
bool umi_test_runtime_command_same_identity(const UmiTestRuntimeCommand *left, const UmiTestRuntimeCommand *right);

#ifdef __cplusplus
}
#endif
#endif
