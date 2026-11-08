/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_submodule_cli.c
 *
 * PURPOSE:
 *   Verify read-only submodule CLI admission and incomplete-evidence handling without running Git.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/submodule_probe.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static unsigned calls;
static int request_valid;
static const char *scenario;
static const char *expected_root = ".";
static UmiStatus CaptureFixture(const UmiRepositoryInspectionContext *context,
    const UmiCancellationToken *cancel, UmiRepositoryGitCaptureResult *out)
{
    ++calls;
    request_valid = context && !strcmp(context->repository_root, expected_root) &&
        !strcmp(context->git_program, "git") && context->timeout_ms == 10000U && !cancel;
    const char *text = " 0123456789012345678901234567890123456789 framework\n";
    if (!strcmp(scenario, "empty")) text = "";
    if (!strcmp(scenario, "mismatch")) text = "+0123456789012345678901234567890123456789 framework\n";
    if (!strcmp(scenario, "malformed")) text = "warning: not a status line\n";
    if (!strcmp(scenario, "failure")) text = "child error\x1b[31m\n";
    memset(out, 0, sizeof(*out));
    out->length = strlen(text); out->bytes = malloc(out->length + 1U);
    if (!out->bytes) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(out->bytes, text, out->length + 1U); out->launched = 1;
    out->exit_code = !strcmp(scenario, "failure") ? 7 : 0;
    out->status = out->exit_code ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
    return out->status;
}
#define UmiRepositorySubmoduleCapture CaptureFixture
#include "../../tools/umicom/src/command_repository_submodules.c"
#undef UmiRepositorySubmoduleCapture
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    scenario = argv[1];
    char *values[3] = {NULL}; char **args = values;
    int count = 0, expected = 2; unsigned expected_calls = 0U;
    if (!strcmp(scenario, "default") || !strcmp(scenario, "empty") || !strcmp(scenario, "mismatch")) { expected = 0; expected_calls = 1U; }
    else if (!strcmp(scenario, "root")) { values[0] = "repository with spaces"; count = 1; expected_root = values[0]; expected = 0; expected_calls = 1U; }
    else if (!strcmp(scenario, "help")) { values[0] = "--help"; count = 1; expected = 0; }
    else if (!strcmp(scenario, "short-help")) { values[0] = "-h"; count = 1; expected = 0; }
    else if (!strcmp(scenario, "failure") || !strcmp(scenario, "malformed")) { expected = 1; expected_calls = 1U; }
    else if (!strcmp(scenario, "negative")) count = -1;
    else if (!strcmp(scenario, "null-vector")) { count = 1; args = NULL; }
    else if (!strcmp(scenario, "null-value")) count = 1;
    else if (!strcmp(scenario, "empty-value")) { count = 1; values[0] = ""; }
    else if (!strcmp(scenario, "extra")) { count = 2; values[0] = "a"; values[1] = "b"; }
    else if (!strcmp(scenario, "option")) { count = 1; values[0] = "--update"; }
    else if (!strcmp(scenario, "help-null")) { count = 2; values[0] = "--help"; }
    else return 2;
    CHECK(umi_cli_command_repository_submodules(count, args) == expected);
    CHECK(calls == expected_calls);
    CHECK(calls == 0U || request_valid);
    return 0;
}
