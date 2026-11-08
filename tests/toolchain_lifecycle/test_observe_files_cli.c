/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_lifecycle/test_observe_files_cli.c
 *
 * PURPOSE:
 *   Verify selected-file command admission before storage work begins.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/toolchain/file_observation.h"
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
static unsigned captures;
static size_t capturedCount, capturedLimit;
static UmiStatus Capture(const char *const *paths, size_t count, size_t maximum,
    UmiFileObservation **out, size_t *failed)
{
    assert(paths != NULL && out != NULL && *out == NULL);
    ++captures; capturedCount = count; capturedLimit = maximum;
    *failed = 0U;
    return UMI_STATUS_BUSY;
}
#define UmiFileObservationCapture Capture
#include "../../tools/umicom/src/command_observe_files.c"
#undef UmiFileObservationCapture
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
#ifdef _WIN32
    char selected[] = "C:/explicit/input.bin", report[] = "C:/explicit/report.json";
#else
    char selected[] = "/explicit/input.bin", report[] = "/explicit/report.json";
#endif
    char *args[] = {"--file", selected, "--max-bytes", "16", "--report", report};
    int length = 6, expected = 2;
    const char *mode = argv[1];
    if (!strcmp(mode, "valid")) expected = 1;
    else if (!strcmp(mode, "default-limit")) { length = 2; expected = 1; }
    else if (!strcmp(mode, "help")) { args[0] = "--help"; length = 1; expected = 0; }
    else if (!strcmp(mode, "empty")) length = 0;
    else if (!strcmp(mode, "negative")) length = -1;
    else if (!strcmp(mode, "null-vector")) { assert(umi_cli_command_observe_files(2, NULL) == 2); return 0; }
    else if (!strcmp(mode, "null-value")) args[5] = NULL;
    else if (!strcmp(mode, "empty-value")) args[5] = "";
    else if (!strcmp(mode, "unknown")) args[4] = "--unknown";
    else if (!strcmp(mode, "missing-value")) length = 5;
    else if (!strcmp(mode, "no-files")) { args[0] = "--report"; args[2] = "--max-bytes"; length = 4; }
    else if (!strcmp(mode, "relative-report")) args[5] = "report.json";
    else if (!strcmp(mode, "duplicate-report")) { args[2] = "--report"; args[3] = report; }
    else if (!strcmp(mode, "duplicate-limit")) { args[4] = "--max-bytes"; args[5] = "32"; }
    else if (!strcmp(mode, "zero")) args[3] = "0";
    else if (!strcmp(mode, "negative-limit")) args[3] = "-1";
    else if (!strcmp(mode, "plus-limit")) args[3] = "+1";
    else if (!strcmp(mode, "decimal-limit")) args[3] = "1.5";
    else if (!strcmp(mode, "overflow")) args[3] = "184467440737095516160";
    else if (!strcmp(mode, "excess-limit")) args[3] = "67108865";
    else if (!strcmp(mode, "help-mixed")) args[4] = "--help";
    else if (!strcmp(mode, "count-excess")) {
        char *many[130];
        for (size_t i = 0U; i < 65U; ++i) { many[i * 2U] = "--file"; many[i * 2U + 1U] = selected; }
        assert(umi_cli_command_observe_files(130, many) == 2 && captures == 0U); return 0;
    } else return 2;
    assert(umi_cli_command_observe_files(length, args) == expected);
    assert(captures == (expected == 1 ? 1U : 0U));
    if (captures != 0U) {
        assert(capturedCount == 1U);
        assert(capturedLimit == (length == 2 ? UMI_FILE_OBSERVATION_MAX_BYTES : 16U));
    }
    return 0;
}
