/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_adapter.c
 *
 * PURPOSE:
 *   Use CTest list and run commands while exposing Framework-owned test records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_adapter.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/process.h"
#include "umicom/testing/discovery.h"

UmiStatus umi_ctest_discover(const char *build_directory,
                             UmiTestSuite *suite,
                             size_t *out_discovered)
{
    const char *arguments[3];
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
    int exit_code = 0;
    UmiStatus status;
    if (build_directory == NULL || suite == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    arguments[0] = "--test-dir";
    arguments[1] = build_directory;
    arguments[2] = "-N";
    status = umi_process_capture("ctest",
                                 arguments,
                                 3U,
                                 output,
                                 sizeof(output),
                                 &exit_code);
    if (status != UMI_STATUS_OK || exit_code != 0) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INTERNAL_ERROR;
    }
    return umi_test_discovery_parse_ctest(output,
                                          build_directory,
                                          suite,
                                          out_discovered);
}

UmiStatus umi_ctest_run(const char *build_directory,
                        const char *test_name,
                        UmiTestResult *out_result)
{
    UmiTestCase test_case;
    char arguments[UMI_TEST_COMMAND_CAPACITY];
    if (build_directory == NULL || test_name == NULL ||
        out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_test_case_init(&test_case, test_name, test_name);
    test_case.kind = UMI_TEST_KIND_EXTERNAL;
    (void)snprintf(arguments,
                   sizeof(arguments),
                   "--test-dir \"%s\" -R \"^%s$\" --output-on-failure",
                   build_directory,
                   test_name);
    if (umi_test_case_set_command(&test_case,
                                  "ctest",
                                  arguments,
                                  NULL) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return umi_test_runner_run_case(&test_case, NULL, out_result);
}
