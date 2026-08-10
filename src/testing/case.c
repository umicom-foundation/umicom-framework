/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/case.c
 *
 * PURPOSE:
 *   Implement test-case construction, command assignment and validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/case.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(source) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, strlen(source) + 1U);
    return UMI_STATUS_OK;
}

void umi_test_case_init(UmiTestCase *test_case,
                        const char *test_id,
                        const char *name)
{
    if (test_case == NULL) {
        return;
    }
    (void)memset(test_case, 0, sizeof(*test_case));
    test_case->kind = UMI_TEST_KIND_UNIT;
    test_case->enabled = 1;
    if (test_id != NULL) {
        (void)copy_text(test_case->test_id,
                        sizeof(test_case->test_id),
                        test_id);
    }
    if (name != NULL) {
        (void)copy_text(test_case->name,
                        sizeof(test_case->name),
                        name);
    }
}

UmiStatus umi_test_case_set_command(UmiTestCase *test_case,
                                    const char *program,
                                    const char *argument_text,
                                    const char *working_directory)
{
    UmiStatus status;
    if (test_case == NULL || program == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_text(test_case->program,
                       sizeof(test_case->program),
                       program);
    if (status == UMI_STATUS_OK && argument_text != NULL) {
        status = copy_text(test_case->argument_text,
                           sizeof(test_case->argument_text),
                           argument_text);
    }
    if (status == UMI_STATUS_OK && working_directory != NULL) {
        status = copy_text(test_case->working_directory,
                           sizeof(test_case->working_directory),
                           working_directory);
    }
    return status;
}

UmiStatus umi_test_case_validate(const UmiTestCase *test_case,
                                 char *out_message,
                                 size_t capacity)
{
    const char *message = "Test case is valid";
    UmiStatus status = UMI_STATUS_OK;
    if (test_case == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (test_case->test_id[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Test identifier is empty";
    } else if (test_case->name[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Test name is empty";
    } else if (test_case->program[0] == '\0') {
        status = UMI_STATUS_INVALID_ARGUMENT;
        message = "Test program is empty";
    }
    if (out_message != NULL && capacity > 0U) {
        (void)snprintf(out_message, capacity, "%s", message);
    }
    return status;
}
