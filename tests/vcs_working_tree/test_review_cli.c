/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/test_review_cli.c
 *
 * PURPOSE:
 *   Verify repository review refuses malformed options before invoking the shared observation service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "../../tools/umicom/src/cli.h"
static unsigned reads;
static const char *expected_root = ".";
static uint32_t expected_timeout;
static UmiStatus read_result = UMI_STATUS_OK;
static int request_valid;
static UmiStatus ReadFixture(const UmiVcsWorkingTreeRequest *request, UmiVcsWorkingTree **out)
{
    ++reads;
    request_valid = request && request->repository_root &&
        !strcmp(request->repository_root, expected_root) && request->timeout_ms == expected_timeout;
    if (read_result != UMI_STATUS_OK) return read_result;
    static const char input[] = HEADERS CHILD "framework\0" ORDINARY "main.c\0";
    return UmiVcsWorkingTreeParse(input, sizeof(input) - 1U, out);
}
#define UmiVcsWorkingTreeRead ReadFixture
#include "../../tools/umicom/src/command_repository_review.c"
#undef UmiVcsWorkingTreeRead
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    char *options[6] = {NULL}; char **args = options;
    int count = 0, expected = 2; unsigned expected_reads = 0U;
    if (!strcmp(mode, "default")) { expected = 0; expected_reads = 1U; }
    else if (!strcmp(mode, "root")) {
        options[0] = "--root"; options[1] = "path with spaces"; count = 2;
        expected_root = options[1]; expected = 0; expected_reads = 1U;
    } else if (!strcmp(mode, "positional")) {
        options[0] = "path with spaces"; count = 1;
        expected_root = options[0]; expected = 0; expected_reads = 1U;
    } else if (!strcmp(mode, "children")) { options[0] = "--children"; count = 1; expected = 0; expected_reads = 1U; }
    else if (!strcmp(mode, "timeout")) {
        options[0] = "--timeout-ms"; options[1] = "15"; count = 2;
        expected_timeout = 15U; expected = 0; expected_reads = 1U;
    } else if (!strcmp(mode, "read-failure")) { read_result = UMI_STATUS_IO_ERROR; expected = 1; expected_reads = 1U; }
    else if (!strcmp(mode, "help")) { options[0] = "--help"; count = 1; expected = 0; }
    else if (!strcmp(mode, "null-vector")) { count = 1; args = NULL; }
    else if (!strcmp(mode, "null-value")) count = 1;
    else if (!strcmp(mode, "empty-value")) { options[0] = ""; count = 1; }
    else if (!strcmp(mode, "negative-count")) count = -1;
    else if (!strcmp(mode, "root-missing")) { options[0] = "--root"; count = 1; }
    else if (!strcmp(mode, "root-option")) { options[0] = "--root"; options[1] = "--children"; count = 2; }
    else if (!strcmp(mode, "root-null")) { options[0] = "--root"; count = 2; }
    else if (!strcmp(mode, "root-duplicate")) { options[0] = "--root"; options[1] = "."; options[2] = "--root"; options[3] = "."; count = 4; }
    else if (!strcmp(mode, "timeout-missing")) { options[0] = "--timeout-ms"; count = 1; }
    else if (!strcmp(mode, "timeout-null")) { options[0] = "--timeout-ms"; count = 2; }
    else if (!strcmp(mode, "timeout-zero")) { options[0] = "--timeout-ms"; options[1] = "0"; count = 2; }
    else if (!strcmp(mode, "timeout-overflow")) { options[0] = "--timeout-ms"; options[1] = "4294967296"; count = 2; }
    else if (!strcmp(mode, "timeout-negative")) { options[0] = "--timeout-ms"; options[1] = "-1"; count = 2; }
    else if (!strcmp(mode, "children-duplicate")) { options[0] = "--children"; options[1] = "--children"; count = 2; }
    else if (!strcmp(mode, "unknown")) { options[0] = "--unknown"; count = 1; }
    else return 2;
    int result = umi_cli_command_repository_review(count, args);
    CHECK(result == expected && reads == expected_reads);
    CHECK(expected_reads == 0U || request_valid);
    return 0;
}
