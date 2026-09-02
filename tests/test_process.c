/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_process.c
 *
 * PURPOSE:
 *   Verify argument-based child-process execution and captured output without
 *   product-specific shell command construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdlib.h>
#include <string.h>

/*
 * Exercise verify capture and return a clear result when the behaviour no longer matches
 * its contract.
 */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_process_capture(program,
                            arguments,
                            sizeof(arguments) / sizeof(arguments[0]),
                            output,
                            sizeof(output),
                            &exit_code) != UMI_STATUS_OK)
        return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (exit_code != 0 || strstr(output, "umicom-process-ok") == NULL)
        return 0;
    return 1;
}

/*
 * Exercise verify hidden request and return a clear result when the behaviour no longer
 * matches its contract.
 */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_process_execute(&request, &result) != UMI_STATUS_OK) return 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!result.launched || result.exit_code != 0) return 0;
    return strstr(result.output, "umicom-hidden-ok") != NULL;
}

/*
 * Exercise verify window mode validation and return a clear result when the behaviour no
 * longer matches its contract.
 */
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

/*
 * Exercise verify long output keeps final diagnostic and return a clear result when the
 * behaviour no longer matches its contract.
 */
static int verify_long_output_keeps_final_diagnostic(void)
{
    UmiProcessRequest request;
    UmiProcessResult result;
#ifdef _WIN32
    const char *arguments[] = {
        "/D", "/S", "/C",
        "for /L %i in (1,1,7000) do @echo umicom-prefix-%i & "
        "echo umicom-final-diagnostic"
    };
    const char *program = "cmd.exe";
#else
    const char *arguments[] = {
        "-c",
        "i=0; while [ \"$i\" -lt 7000 ]; do "
        "printf 'umicom-prefix-%s\\n' \"$i\"; i=$((i+1)); done; "
        "printf umicom-final-diagnostic"
    };
    const char *program = "/bin/sh";
#endif

    (void)memset(&request, 0, sizeof(request));
    request.program = program;
    request.arguments = arguments;
    request.argument_count = sizeof(arguments) / sizeof(arguments[0]);
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_process_execute(&request, &result) != UMI_STATUS_OK) return 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.exit_code != 0 || !result.output_truncated) return 0;
    return strstr(result.output, "umicom-final-diagnostic") != NULL;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (!verify_capture()) return EXIT_FAILURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!verify_hidden_request()) return EXIT_FAILURE;
    /* Apply this operation only while the related capability or state is available. */
    if (!verify_window_mode_validation()) return EXIT_FAILURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!verify_long_output_keeps_final_diagnostic()) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
