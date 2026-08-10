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

int main(void)
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
        return EXIT_FAILURE;
    if (exit_code != 0 || strstr(output, "umicom-process-ok") == NULL)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
