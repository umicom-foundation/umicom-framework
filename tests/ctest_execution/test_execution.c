/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_execution/test_execution.c
 *
 * PURPOSE:
 *   Launch real CTest through the production Framework process implementation
 *   and verify one selected-test scenario per invocation of this test program.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int Exists(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return 0;
    fclose(file); return 1;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: umicom-ctest-execution <fixture-build> <scenario>\n");
        return 2;
    }
    const char *directory = argv[1], *scenario = argv[2], *name = "notes.pass";
    char marker[4096], alternate[4096], longName[UMI_TEST_NAME_CAPACITY];
    if (snprintf(marker, sizeof marker, "%s/unexpected.marker", directory) < 0 ||
        strlen(directory) + 32U >= sizeof marker) return 2;
    (void)remove(marker); /* This marker belongs exclusively to this fixture. */
    UmiCtestRunOptions options = {0};
    options.test_id = "umicom.execution.selected";
    UmiTestResult result;
    UmiStatus expectedStatus = UMI_STATUS_OK;
    UmiTestState expectedState = UMI_TEST_STATE_PASSED;
    UmiCancellationToken *token = NULL;
    int allowAnyError = 0, legacy = 0, expectTail = 0;
    if (strcmp(scenario, "literal-metacharacters") == 0) name = "notes.save[1]+";
    else if (strcmp(scenario, "literal-dot") == 0) name = "notes.save";
    else if (strcmp(scenario, "quoted-name") == 0) name = "notes.\"quoted\" & value";
    else if (strcmp(scenario, "backslash-name") == 0) name = "notes.back\\slash";
    else if (strcmp(scenario, "semicolon-name") == 0) name = "notes;semicolon";
    else if (strcmp(scenario, "unicode-name") == 0) name = "notes.\xC3\xA9";
    else if (strcmp(scenario, "long-name") == 0) {
        memcpy(longName, "notes.", 6U);
        memset(longName + 6U, 'x', sizeof longName - 7U);
        longName[sizeof longName - 1U] = '\0'; name = longName;
    } else if (strcmp(scenario, "working-directory") == 0) name = "notes.working-directory";
    else if (strcmp(scenario, "configuration") == 0) {
        name = "notes.configuration"; options.configuration = "Debug";
    } else if (strcmp(scenario, "wrong-configuration") == 0) {
        name = "notes.configuration"; options.configuration = "Release";
        expectedStatus = UMI_STATUS_NOT_FOUND; expectedState = UMI_TEST_STATE_NOT_RUN;
    } else if (strcmp(scenario, "test-failure") == 0) {
        name = "notes.fail"; expectedStatus = UMI_STATUS_INTERNAL_ERROR;
        expectedState = UMI_TEST_STATE_FAILED;
    } else if (strcmp(scenario, "skip-return-code") == 0) {
        name = "notes.skip"; expectedState = UMI_TEST_STATE_SKIPPED;
    } else if (strcmp(scenario, "skip-expression") == 0) {
        name = "notes.regex-skip"; expectedState = UMI_TEST_STATE_SKIPPED;
    } else if (strcmp(scenario, "disabled") == 0) {
        name = "notes.disabled"; expectedState = UMI_TEST_STATE_SKIPPED;
        /* With --no-tests=error an entirely disabled selection has no executed
         * test, so CTest itself also fails. Retain both facts rather than PASS. */
        expectedStatus = UMI_STATUS_INTERNAL_ERROR;
    } else if (strcmp(scenario, "missing-executable") == 0) {
        name = "notes.missing"; expectedState = UMI_TEST_STATE_NOT_RUN;
        expectedStatus = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(scenario, "missing-selection") == 0) {
        name = "notes.not-in-this-catalogue"; expectedState = UMI_TEST_STATE_NOT_RUN;
        expectedStatus = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(scenario, "test-timeout") == 0 || strcmp(scenario, "process-timeout") == 0) {
        name = strcmp(scenario, "test-timeout") == 0 ? "notes.timeout" : "notes.process-timeout";
        if (strcmp(scenario, "process-timeout") == 0) options.timeout_ms = 50U;
        expectedState = UMI_TEST_STATE_TIMED_OUT; expectedStatus = UMI_STATUS_TIMEOUT;
    } else if (strcmp(scenario, "cancel-before-launch") == 0) {
        if (umi_cancellation_token_create(&token) != UMI_STATUS_OK) return 2;
        umi_cancellation_token_request(token); options.cancellation = token;
        expectedState = UMI_TEST_STATE_CANCELLED; expectedStatus = UMI_STATUS_CANCELLED;
    } else if (strcmp(scenario, "fixture-lifecycle") == 0) name = "notes.fixture-body";
    else if (strcmp(scenario, "failed-setup") == 0) {
        name = "notes.blocked-body"; expectedState = UMI_TEST_STATE_NOT_RUN;
        expectedStatus = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(scenario, "failed-cleanup") == 0) {
        name = "notes.cleanup-body"; expectedState = UMI_TEST_STATE_FAILED;
        expectedStatus = UMI_STATUS_INTERNAL_ERROR;
    } else if (strcmp(scenario, "large-output") == 0) {
        name = "notes.large-output"; expectedState = UMI_TEST_STATE_FAILED;
        expectedStatus = UMI_STATUS_INTERNAL_ERROR; expectTail = 1;
    } else if (strcmp(scenario, "wrong-build-root") == 0) {
        snprintf(alternate, sizeof alternate, "%s/test-working-directory", directory);
        directory = alternate; expectedState = UMI_TEST_STATE_NOT_RUN; allowAnyError = 1;
    } else if (strcmp(scenario, "legacy-entry") == 0) { name = "notes.save[1]+"; legacy = 1; }
    else if (strcmp(scenario, "aliased-input") == 0) {
        memset(&result, 0, sizeof result);
        strcpy(result.name, "notes.pass"); strcpy(result.test_id, "umicom.alias");
        name = result.name; options.test_id = result.test_id;
    } else if (strcmp(scenario, "invalid-request") == 0) {
        name = NULL; expectedStatus = UMI_STATUS_INVALID_ARGUMENT;
        expectedState = UMI_TEST_STATE_NOT_RUN;
    } else if (strcmp(scenario, "pass") != 0) {
        fprintf(stderr, "Unknown scenario: %s\n", scenario); return 2;
    }
    UmiStatus status = legacy ? umi_ctest_run(directory, name, &result)
        : UmiCtestRunConfigured(directory, name, &options, &result);
    umi_cancellation_token_destroy(token);
    int failed = (allowAnyError ? status == UMI_STATUS_OK : status != expectedStatus)
        || result.status != status || result.state != expectedState || Exists(marker);
    if (expectedState == UMI_TEST_STATE_PASSED && !allowAnyError)
        failed |= UmiCtestResultRequirePassed(&result) != UMI_STATUS_OK;
    else failed |= UmiCtestResultRequirePassed(&result) == UMI_STATUS_OK;
    if (expectTail) failed |= strstr(result.output, "UMICOM_TAIL_MARKER") == NULL ||
        strstr(result.output, "Output truncated") == NULL;
    if (strcmp(scenario, "aliased-input") == 0)
        failed |= strcmp(result.name, "notes.pass") != 0 || strcmp(result.test_id, "umicom.alias") != 0;
    if (strcmp(scenario, "invalid-request") == 0)
        failed |= result.exit_code != -1 || result.output[0] != '\0';
    if (strcmp(scenario, "fixture-lifecycle") == 0) {
        size_t used = strlen(directory);
        if (used + sizeof "/fixture.flag" > sizeof alternate) return 2;
        memmove(alternate, directory, used);
        memcpy(alternate + used, "/fixture.flag", sizeof "/fixture.flag");
        failed |= Exists(alternate);
    }
    printf("%s: status=%d state=%d exit=%d outcome=%s\n", scenario, (int)status,
        (int)result.state, result.exit_code, failed ? "FAIL" : "PASS");
    if (failed) fprintf(stderr, "%s\n", result.output);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
