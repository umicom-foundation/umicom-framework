/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/owned_build_command/main.c
 *
 * PURPOSE:
 *   Inspect, copy and refuse a malformed owned command through an installed Framework SDK without launching a process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/build/types.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    const char *mode = argc == 2 ? argv[1] : "";
    if (strcmp(mode, "inspect") && strcmp(mode, "copy") && strcmp(mode, "refusal")) {
        fprintf(stderr, "usage: owned-build-command inspect|copy|refusal\n");
        return 2;
    }
    UmiBuildCommand current, saved;
    char message[256], display[1024];
    UmiStatus status = umi_build_command_parse(
        "cmake --build 'build folder' --config Debug", &current, message, sizeof message);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot prepare command: %s\n", message); return 1;
    }
    if (!strcmp(mode, "copy")) {
        status = umi_build_command_copy(&saved, &current);
        if (status != UMI_STATUS_OK) return 1;
        /* Overwriting the original demonstrates the copied record's ownership. */
        memset(&current, 0, sizeof current);
        if (saved.arguments[1] != saved.argument_storage[1] ||
            strcmp(saved.arguments[1], "build folder")) return 1;
        if (!umi_build_command_format(&saved, display, sizeof display)) return 1;
    } else if (!strcmp(mode, "refusal")) {
        memcpy(&saved, &current, sizeof saved); /* Byte snapshot; argv is not consumed. */
        status = umi_build_command_parse("cmake 'unfinished", &current, message, sizeof message);
        if (status != UMI_STATUS_PARSE_ERROR || memcmp(&saved, &current, sizeof current)) return 1;
        printf("Refused draft: %s\n", message);
        if (!umi_build_command_format(&current, display, sizeof display)) return 1;
    } else {
        for (size_t i = 0U; i < current.argument_count; ++i)
            printf("argv[%zu] = <%s>\n", i + 1U, current.arguments[i]);
        if (!umi_build_command_format(&current, display, sizeof display)) return 1;
    }
    printf("Display only: %s\n", display);
    puts("No build or child process was started.");
    return 0;
}
