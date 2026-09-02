/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/case.c
 *
 * PURPOSE:
 *   Implement test-case construction, command assignment and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/case.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(source) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, strlen(source) + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise test case from caller-provided values so later operations receive a known
 * state.
 */
void umi_test_case_init(UmiTestCase *test_case,
                        const char *test_id,
                        const char *name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (test_case == NULL) {
        return;
    }
    (void)memset(test_case, 0, sizeof(*test_case));
    test_case->kind = UMI_TEST_KIND_UNIT;
    test_case->enabled = 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (test_id != NULL) {
        (void)copy_text(test_case->test_id,
                        sizeof(test_case->test_id),
                        test_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name != NULL) {
        (void)copy_text(test_case->name,
                        sizeof(test_case->name),
                        name);
    }
}

/*
 * Provide the test case set command operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_case_set_command(UmiTestCase *test_case,
                                    const char *program,
                                    const char *argument_text,
                                    const char *working_directory)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (test_case == NULL || program == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_text(test_case->program,
                       sizeof(test_case->program),
                       program);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && argument_text != NULL) {
        status = copy_text(test_case->argument_text,
                           sizeof(test_case->argument_text),
                           argument_text);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && working_directory != NULL) {
        status = copy_text(test_case->working_directory,
                           sizeof(test_case->working_directory),
                           working_directory);
    }
    return status;
}

/* Check that test case satisfies its contract before another service relies on it. */
UmiStatus umi_test_case_validate(const UmiTestCase *test_case,
                                 char *out_message,
                                 size_t capacity)
{
    const char *message = "Test case is valid";
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (test_case == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (test_case->test_id[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Test identifier is empty";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (test_case->name[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Test name is empty";
    } else /* Apply this branch only when its contract condition is satisfied. */ if (test_case->program[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Test program is empty";
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", message);
    }
    return status;
}
