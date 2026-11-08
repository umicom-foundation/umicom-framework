/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_submodule_probe.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the submodule probe capability.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/submodule_probe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASH "0123456789012345678901234567890123456789"
#define WIDE HASH "012345678901234567890123"
#define ROW " " HASH " framework (heads/main)\n"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static unsigned capture_calls;
static int command_valid;
static UmiStatus fixture_status = UMI_STATUS_OK;
static UmiStatus CaptureFixture(const UmiRepositoryInspectionContext *context,
    const char *const *arguments, size_t count, size_t capacity,
    const UmiCancellationToken *cancel, UmiRepositoryGitCaptureResult *out)
{
    ++capture_calls;
    const char *expected[] = {"--no-optional-locks", "-c", "core.quotepath=true",
        "-c", "color.ui=false", "submodule", "status", "--recursive"};
    command_valid = context && !strcmp(context->repository_root, "repo with spaces") &&
        !strcmp(context->git_program, "chosen-git") && context->timeout_ms == 1234U &&
        count == sizeof(expected) / sizeof(expected[0]) && capacity == UMI_REPOSITORY_GIT_CAPTURE_LIMIT && !cancel;
    for (size_t i = 0U; command_valid && i < count; ++i)
        if (strcmp(arguments[i], expected[i])) command_valid = 0;
    memset(out, 0, sizeof(*out));
    out->bytes = malloc(sizeof(ROW));
    if (!out->bytes) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(out->bytes, ROW, sizeof(ROW)); out->length = sizeof(ROW) - 1U;
    out->launched = 1; out->exit_code = fixture_status == UMI_STATUS_OK ? 0 : 7;
    out->status = fixture_status;
    return fixture_status;
}
#define UmiRepositoryGitCapture CaptureFixture
#include "../../src/repository/inspection/submodule_probe.c"
#undef UmiRepositoryGitCapture

/* The earlier symbol-only smoke check is retained. Observable contract cases
 * below replace it so malformed or partial evidence cannot produce a pass. */
#if 0


#include "umicom/repository/submodule_probe.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    (void)&umi_repository_submodule_probe_read; return 0;
}

#endif

int main(int argc, char **argv)
{
    const char *mode = argc == 1 ? "capture" : argc == 2 ? argv[1] : "invalid";
    UmiRepositorySubmoduleStatus output, before;
    memset(&output, 0x5a, sizeof(output)); before = output;
    UmiStatus expected = UMI_STATUS_PARSE_ERROR, status;
    const unsigned char *input = (const unsigned char *)ROW;
    size_t length = sizeof(ROW) - 1U;
    unsigned char scratch[6000];
    if (!strcmp(mode, "capture") || !strcmp(mode, "failed-capture")) {
        UmiRepositoryInspectionContext context = {"chosen-git", "repo with spaces", 1234U};
        if (!strcmp(mode, "failed-capture")) fixture_status = UMI_STATUS_IO_ERROR;
        status = umi_repository_submodule_probe_read(&context, &output);
        CHECK(status == fixture_status && capture_calls == 1U && command_valid);
        if (status == UMI_STATUS_OK) CHECK(output.configured == 1U && output.initialised == 1U);
        else CHECK(output.configured == 0U && output.initialised == 0U);
        return 0;
    }
    if (!strcmp(mode, "null")) {
        CHECK(UmiRepositorySubmoduleStatusParse(NULL, 1U, &output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before, &output, sizeof(output)) == 0);
        CHECK(UmiRepositorySubmoduleStatusParse(input, length, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (!strcmp(mode, "capacity")) {
        CHECK(UmiRepositorySubmoduleStatusParse(input, UMI_REPOSITORY_GIT_CAPTURE_LIMIT + 1U, &output) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&before, &output, sizeof(output)) == 0);
        return 0;
    }
    if (!strcmp(mode, "empty")) { input = NULL; length = 0U; expected = UMI_STATUS_OK; }
    else if (!strcmp(mode, "markers")) {
        input = (const unsigned char *)(ROW "-" HASH " missing\n+" HASH " changed\nU" HASH " conflicted\n");
        length = strlen((const char *)input); expected = UMI_STATUS_OK;
    } else if (!strcmp(mode, "wide")) {
        input = (const unsigned char *)(" " WIDE " nested/path with spaces (v1)\n");
        length = strlen((const char *)input); expected = UMI_STATUS_OK;
    } else if (!strcmp(mode, "crlf")) {
        input = (const unsigned char *)(" " HASH " child\r\n");
        length = strlen((const char *)input); expected = UMI_STATUS_OK;
    } else if (!strcmp(mode, "unterminated")) --length;
    else if (!strcmp(mode, "warning")) {
        input = (const unsigned char *)(ROW "warning: diagnostic output\n"); length = strlen((const char *)input);
    } else if (!strcmp(mode, "blank")) { input = (const unsigned char *)(ROW "\n"); length = strlen((const char *)input); }
    else if (!strcmp(mode, "unknown")) { memcpy(scratch, input, length); scratch[0] = '?'; input = scratch; }
    else if (!strcmp(mode, "nonhex")) { memcpy(scratch, input, length); scratch[8] = 'z'; input = scratch; }
    else if (!strcmp(mode, "short-hash")) { input = (const unsigned char *)" 0123 child\n"; length = strlen((const char *)input); }
    else if (!strcmp(mode, "long-hash")) { input = (const unsigned char *)(" " WIDE "0 child\n"); length = strlen((const char *)input); }
    else if (!strcmp(mode, "no-path")) { input = (const unsigned char *)(" " HASH " \n"); length = strlen((const char *)input); }
    else if (!strcmp(mode, "control") || !strcmp(mode, "nul")) {
        memcpy(scratch, input, length); scratch[44] = !strcmp(mode, "nul") ? 0U : 0x1bU; input = scratch;
    } else if (!strcmp(mode, "oversized-line")) {
        memcpy(scratch, " " HASH " ", 42U); memset(scratch + 42U, 'x', 5000U);
        scratch[5042] = '\n'; length = 5043U; input = scratch; expected = UMI_STATUS_OK;
    } else return 2;
    status = UmiRepositorySubmoduleStatusParse(input, length, &output);
    CHECK(status == expected);
    if (status != UMI_STATUS_OK) CHECK(memcmp(&before, &output, sizeof(output)) == 0);
    else if (!strcmp(mode, "empty")) CHECK(output.configured == 0U && output.initialised == 0U);
    else if (!strcmp(mode, "markers")) CHECK(output.configured == 4U && output.initialised == 3U && output.missing == 1U && output.head_mismatch == 1U && output.dirty == 1U);
    else CHECK(output.configured == 1U && output.initialised == 1U && output.missing == 0U);
    return 0;
}
