/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_process/test_windows_arguments.c
 * PURPOSE: Check the actual Windows argument encoder on every build platform.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/process.h"
#include <stdio.h>
#include <string.h>
#include "../../src/platform/process_windows_arguments.inc"

int main(int argc, char **argv)
{
    const char *values[] = {"", "plain", "two words", "a\"b", "two words\\", "x\\\"y", "\\", "\xCE\xBB"};
    const char *encoded[] = {"\"\"", "plain", "\"two words\"", "\"a\\\"b\"", "\"two words\\\\\"", "\"x\\\\\\\"y\"", "\\", "\xCE\xBB"};
    if (argc != 2) return 2;
    for (size_t i = 0U; i < sizeof(values) / sizeof(values[0]); ++i) {
        const char *arguments[] = {values[i]};
        UmiProcessRequest request = {0};
        request.program = "tool.exe"; request.arguments = arguments; request.argument_count = 1U;
        char expected[256];
        (void)snprintf(expected, sizeof(expected), "tool.exe %s", encoded[i]);
        size_t length = strlen(expected);
        if (strcmp(argv[1], "cases") == 0) {
            char output[256];
            if (umi_windows_command_line(&request, output, sizeof(output)) != UMI_STATUS_OK ||
                strcmp(output, expected) != 0) return 1;
        } else if (strcmp(argv[1], "bounds") == 0) {
            for (size_t capacity = 0U; capacity <= length + 2U; ++capacity) {
                unsigned char guarded[300];
                memset(guarded, 0x5a, sizeof(guarded));
                UmiStatus status = umi_windows_command_line(&request, (char *)guarded + 1U, capacity);
                if ((capacity > length && (status != UMI_STATUS_OK || strcmp((char *)guarded + 1U, expected) != 0)) ||
                    (capacity <= length && status != UMI_STATUS_CAPACITY_EXCEEDED)) return 1;
                if (guarded[0] != 0x5a || guarded[capacity + 1U] != 0x5a) return 1;
            }
        } else return 2;
    }
    return 0;
}
