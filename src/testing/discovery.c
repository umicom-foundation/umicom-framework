/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/discovery.c
 *
 * PURPOSE:
 *   Parse CTest list output into executable Framework test cases.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/discovery.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_test_discovery_parse_ctest(const char *output,
                                         const char *build_directory,
                                         UmiTestSuite *suite,
                                         size_t *out_discovered)
{
    const char *cursor;
    const char *end;
    char line[1024];
    char name[UMI_TEST_NAME_CAPACITY];
    size_t length;
    size_t discovered = 0U;
    unsigned number;

    if (output == NULL || build_directory == NULL || suite == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    cursor = output;
    while (*cursor != '\0') {
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        if (sscanf(line, "  Test #%u: %255[^\r\n]", &number, name) == 2 ||
            sscanf(line, "Test #%u: %255[^\r\n]", &number, name) == 2) {
            UmiTestCase test_case;
            (void)number;
            umi_test_case_init(&test_case, name, name);
            test_case.kind = UMI_TEST_KIND_EXTERNAL;
            test_case.timeout_ms = 0U;
            if (umi_test_case_set_command(&test_case,
                                          "ctest",
                                          name,
                                          build_directory) ==
                    UMI_STATUS_OK &&
                umi_test_suite_add(suite, &test_case) == UMI_STATUS_OK) {
                discovered += 1U;
            }
        }
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
    if (out_discovered != NULL) {
        *out_discovered = discovered;
    }
    return UMI_STATUS_OK;
}
