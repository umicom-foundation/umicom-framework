/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/discovery.c
 *
 * PURPOSE:
 *   Parse CTest list output into executable Framework test cases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/discovery.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the test discovery parse ctest operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || build_directory == NULL || suite == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    cursor = output;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        end = strchr(cursor, '\n');
        length = end != NULL ? (size_t)(end - cursor) : strlen(cursor);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(line)) {
            length = sizeof(line) - 1U;
        }
        (void)memcpy(line, cursor, length);
        line[length] = '\0';
        /* Apply this branch only when its contract condition is satisfied. */
        if (sscanf(line, "  Test #%u: %255[^\r\n]", &number, name) == 2 ||
            sscanf(line, "Test #%u: %255[^\r\n]", &number, name) == 2) {
            UmiTestCase test_case;
            (void)number;
            umi_test_case_init(&test_case, name, name);
            test_case.kind = UMI_TEST_KIND_EXTERNAL;
            test_case.timeout_ms = 0U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_test_case_set_command(&test_case,
                                          "ctest",
                                          name,
                                          build_directory) ==
                    UMI_STATUS_OK &&
                umi_test_suite_add(suite, &test_case) == UMI_STATUS_OK) {
                discovered += 1U;
            }
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_discovered != NULL) {
        *out_discovered = discovered;
    }
    return UMI_STATUS_OK;
}
