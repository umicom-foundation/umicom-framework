/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/case.h
 *
 * PURPOSE:
 *   Represent one executable test case with owned metadata and an explicit process command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_CASE_H
#define UMICOM_TESTING_CASE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/testing/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test case data shared with callers of this public contract.
 */
typedef struct UmiTestCase {
    char test_id[UMI_TEST_ID_CAPACITY];
    char name[UMI_TEST_NAME_CAPACITY];
    char labels[UMI_TEST_LABEL_CAPACITY];
    char program[UMI_TEST_COMMAND_CAPACITY];
    char working_directory[UMI_TEST_COMMAND_CAPACITY];
    char argument_text[UMI_TEST_COMMAND_CAPACITY];
    UmiTestKind kind;
    uint32_t timeout_ms;
    int enabled;
} UmiTestCase;

/**
 * Initialise test case from caller-provided values so later operations receive a known
 * state.
 */
void umi_test_case_init(UmiTestCase *test_case,
                        const char *test_id,
                        const char *name);
/**
 * Provide the test case set command operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_case_set_command(UmiTestCase *test_case,
                                    const char *program,
                                    const char *argument_text,
                                    const char *working_directory);
/**
 * Check that test case satisfies its contract before another service relies on it.
 */
UmiStatus umi_test_case_validate(const UmiTestCase *test_case,
                                 char *out_message,
                                 size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
