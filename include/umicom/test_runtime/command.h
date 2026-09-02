/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/command.h
 *
 * PURPOSE:
 *   Describe typed Master Controller requests for test-runtime operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_COMMAND
#define UMICOM_TEST_RUNTIME_COMMAND

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime command data shared with callers of this public contract.
 */
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

/**
 * Initialise test runtime command from caller-provided values so later operations receive
 * a known state.
 */
void umi_test_runtime_command_init(UmiTestRuntimeCommand *value, const char *id);
/**
 * Check that test runtime command satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_test_runtime_command_validate(const UmiTestRuntimeCommand *value);
/**
 * Provide the test runtime command set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_command_set_name(UmiTestRuntimeCommand *value, const char *name);
/**
 * Provide the test runtime command set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_command_set_detail(UmiTestRuntimeCommand *value, const char *detail);
/**
 * Provide the test runtime command set kind operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_command_set_kind(UmiTestRuntimeCommand *value, uint64_t number);
/**
 * Provide the test runtime command set sequence operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_command_set_sequence(UmiTestRuntimeCommand *value, uint64_t number);
/**
 * Provide the test runtime command touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_command_touch(UmiTestRuntimeCommand *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime command same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_command_same_identity(const UmiTestRuntimeCommand *left, const UmiTestRuntimeCommand *right);

#ifdef __cplusplus
}
#endif
#endif
