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
/**
 * Represent the test runtime command line data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise test runtime command line from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_command_line_init(UmiTestRuntimeCommandLine *value,const char *id);
/**
 * Check that test runtime command line satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_command_line_validate(const UmiTestRuntimeCommandLine *value);
/**
 * Provide the test runtime command line set category operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_command_line_set_category(UmiTestRuntimeCommandLine *value,const char *category);
/**
 * Provide the test runtime command line set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_command_line_set_detail(UmiTestRuntimeCommandLine *value,const char *detail);
/**
 * Return the number of records represented by test runtime command line set argument
 * without changing their state.
 */
UmiStatus umi_test_runtime_command_line_set_argument_count(UmiTestRuntimeCommandLine *value,uint64_t number);
/**
 * Return the number of records represented by test runtime command line set byte without
 * changing their state.
 */
UmiStatus umi_test_runtime_command_line_set_byte_count(UmiTestRuntimeCommandLine *value,uint64_t number);
/**
 * Provide the test runtime command line set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_command_line_set_active(UmiTestRuntimeCommandLine *value,bool active);
/**
 * Provide the test runtime command line same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_command_line_same_identity(const UmiTestRuntimeCommandLine *left,const UmiTestRuntimeCommandLine *right);
/* Text setters publish a complete field and one revision together. Capacity,
 * invalid-input and exhausted-revision refusals preserve the record. Scalar
 * setters also refuse revision exhaustion. Call these on the value's owner;
 * they do not supply locking or persist changes to a storage service. */

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_test_runtime_command_line_init_checked(UmiTestRuntimeCommandLine *value, const char *id);

#ifdef __cplusplus
}
#endif
#endif
