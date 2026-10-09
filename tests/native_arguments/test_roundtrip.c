/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_arguments/test_roundtrip.c
 * PURPOSE: Check native Unicode argument capture through the real platform process launcher.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/native_arguments.h"
#include "umicom/platform/process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include empty values, Unicode, quotes, shell metacharacters and trailing slashes in one native
 * round trip. */
static const char *const payload[] = {
    "--child",         "café",           "漢字",   "\xf0\x9f\x8e\xb5", "",
    "C:\\a folder\\",  "literal\"quote", "a\\\"b", "line\nbreak",      "$HOME;$(never-run)",
    "--leading-option"};

/* The callback sees original Unicode bytes even when Windows supplied a lossy narrow main vector.
 */
static int Child(int argc, char **argv)
{
    if (argc != (int)(sizeof(payload) / sizeof(payload[0])) + 1)
        return 10;
    for (int index = 1; index < argc; ++index)
        if (strcmp(argv[index], payload[index - 1]) != 0)
            return 11;
    /* Frontends such as GTK can reorder argv. The shared owner still frees its own storage. */
    char *first = argv[0];
    argv[0] = argv[1];
    argv[1] = first;
    return 23;
}
int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--child") == 0)
    {
        int result = 1;
        UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, Child, &result);
        return status == UMI_STATUS_OK ? result : 12;
    }
    UmiNativeArguments *arguments = NULL;
    UmiStatus status = UmiNativeArgumentsCapture(argc, argv, &arguments);
    if (status != UMI_STATUS_OK || UmiNativeArgumentsCount(arguments) == 0)
        return 1;
    UmiProcessRequest request = {0};
    UmiProcessResult *result = calloc(1U, sizeof(*result));
    if (result == NULL)
    {
        UmiNativeArgumentsDestroy(arguments);
        return 2;
    }
    request.program = UmiNativeArgumentsValues(arguments)[0];
    request.arguments = payload;
    request.argument_count = sizeof(payload) / sizeof(payload[0]);
    request.capture_stdout = request.capture_stderr = 1;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    request.timeout_ms = 5000U;
    status = UmiProcessExecuteWithLifetime(&request, UMI_PROCESS_LIFETIME_TREE, NULL, NULL, NULL,
                                           result);
    int passed = status == UMI_STATUS_OK && result->launched && result->exit_code == 23 &&
                 !result->cancelled && !result->timed_out;
    if (!passed)
        fprintf(stderr, "Native argument round trip failed: %s, child exit %d\n",
                umi_status_text(status), result->exit_code);
    free(result);
    UmiNativeArgumentsDestroy(arguments);
    return passed ? 0 : 1;
}
