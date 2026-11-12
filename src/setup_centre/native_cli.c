/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/setup_centre/native_cli.c
 *
 * PURPOSE:
 *   Own native setup execution context once for all command-line hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/setup_centre/execution.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int UmiSetupNativeMain(int argc, char **argv)
{
    if (argc < 1 || argv == NULL) return 2;
    for (int i = 0; i < argc; ++i) if (argv[i] == NULL) return 2;
    UmiProcessResult *result = calloc(1U, sizeof *result);
    if (result == NULL) return 1;
    /* The process service owns launch and tree cleanup. The CLI owns this
     * result only for the synchronous invocation and never invokes a shell. */
    UmiSetupProcessContext context = {NULL, 300000U, result};
    if (argc > 2 && strcmp(argv[1], "vm-run") == 0) {
        for (int i = 2; i + 1 < argc; i += 2)
            if (strcmp(argv[i], "--interactive") == 0 && strcmp(argv[i + 1], "yes") == 0)
                context.timeoutMs = 0U;
    }
    int code = UmiSetupMainWithExecutor(argc, argv, UmiSetupExecuteProcess, &context);
    if (result->output[0] != '\0') fputs(result->output, stderr);
    free(result);
    return code;
}
