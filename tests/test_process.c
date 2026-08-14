/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_process.c
 *
 * PURPOSE:
 *   Verify argument-based child-process execution and captured output without
 *   product-specific shell command construction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdlib.h>
#include <string.h>

static int verify_capture(void)
{
    char output[1024];
    int exit_code = -1;
#ifdef _WIN32
    const char *arguments[] = {"/C", "echo", "umicom-process-ok"};
    const char *program = "cmd.exe";
#else
    const char *arguments[] = {"-c", "printf umicom-process-ok"};
    const char *program = "/bin/sh";
#endif
    if (umi_process_capture(program,
                            arguments,
                            sizeof(arguments) / sizeof(arguments[0]),
                            output,
                            sizeof(output),
                            &exit_code) != UMI_STATUS_OK)
        return 0;
    if (exit_code != 0 || strstr(output, "umicom-process-ok") == NULL)
        return 0;
    return 1;
}

static int verify_hidden_request(void)
{
    UmiProcessRequest request;
    UmiProcessResult result;
#ifdef _WIN32
    const char *arguments[] = {"/C", "echo", "umicom-hidden-ok"};
    const char *program = "cmd.exe";
#else
    const char *arguments[] = {"-c", "printf umicom-hidden-ok"};
    const char *program = "/bin/sh";
#endif

    (void)memset(&request, 0, sizeof(request));
    request.program = program;
    request.arguments = arguments;
    request.argument_count = sizeof(arguments) / sizeof(arguments[0]);
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;

    if (umi_process_execute(&request, &result) != UMI_STATUS_OK) return 0;
    if (!result.launched || result.exit_code != 0) return 0;
    return strstr(result.output, "umicom-hidden-ok") != NULL;
}

static int verify_window_mode_validation(void)
{
    UmiProcessRequest request;
    UmiProcessResult result;

    (void)memset(&request, 0, sizeof(request));
    request.program = "unused";
    request.window_mode = (UmiProcessWindowMode)99;
    return umi_process_execute(&request, &result) ==
        UMI_STATUS_INVALID_ARGUMENT;
}

int main(void)
{
    if (!verify_capture()) return EXIT_FAILURE;
    if (!verify_hidden_request()) return EXIT_FAILURE;
    if (!verify_window_mode_validation()) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
