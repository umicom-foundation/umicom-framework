/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/tool_location/test_process.c
 * PURPOSE: Verify native child tool selection, literal arguments and parent environment isolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_search_path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static void Observe(const char *bytes, size_t count, void *context)
{
    if (bytes != NULL)
        *(size_t *)context += count;
}
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1], *directory = argv[2];
    CHECK(umi_path_is_absolute(directory));
    UmiProcessRequest request = {0};
    UmiProcessResult *result = calloc(1U, sizeof *result);
    CHECK(result != NULL);
    const char *arguments[] = {directory, "literal ^&| 'argument'"};
    request.program = "umicom-tool-probe";
    request.arguments = arguments;
    request.argument_count = 2U;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = 5000U;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    char *parentBefore = NULL, *parentAfter = NULL;
    CHECK(UmiProcessSearchPathCapture(directory, &parentBefore) == UMI_STATUS_OK);
    UmiEnvironmentVariable environment[] = {{"PATH", "inherited-marker"},
                                            {"UMICOM_TOOL_PROBE", "child-only"}};
    size_t bytes = 0U;
    if (strcmp(mode, "override") == 0)
    {
        /* Keep the owner's native runtime DLL paths while proving that an
         * explicit PATH override, rather than the parent value, is inherited. */
        environment[0].value = parentBefore;
        request.environment = environment;
        request.environment_count = 2U;
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        environment[1].name = "PATH";
        request.environment = environment;
        request.environment_count = 2U;
        CHECK(UmiProcessExecuteTool(&request, directory, UMI_PROCESS_LIFETIME_TREE, Observe, &bytes,
                                    result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!result->launched && result->exit_code == -1 && bytes == 0U);
        free(parentBefore);
        free(result);
        return 0;
    }
    const char *selected = directory;
    char missing[UMI_PATH_CAPACITY];
    if (strcmp(mode, "missing") == 0)
    {
        CHECK(umi_path_join(directory, "directory-that-does-not-exist", missing, sizeof missing) ==
              UMI_STATUS_OK);
        CHECK(!umi_fs_exists(missing));
        selected = missing;
    }
    UmiStatus status = UmiProcessExecuteTool(&request, selected, UMI_PROCESS_LIFETIME_TREE, Observe,
                                             &bytes, result);
    if (strcmp(mode, "missing") == 0)
    {
        CHECK(status != UMI_STATUS_OK && !result->launched && bytes == 0U);
    }
    else
    {
        CHECK(status == UMI_STATUS_OK && result->launched && result->exit_code == 0);
        CHECK(bytes > 0U && strstr(result->output, "ARG=[literal ^&| 'argument']") != NULL);
        CHECK(strstr(result->output, directory) != NULL);
        if (strcmp(mode, "override") == 0)
        {
            char *expected = NULL;
            CHECK(UmiProcessSearchPathJoin(directory, parentBefore, &expected) == UMI_STATUS_OK);
            CHECK(strstr(result->output, expected) != NULL);
            UmiProcessSearchPathFree(expected);
            CHECK(strstr(result->output, "MARKER=[child-only]") != NULL);
        }
        else
            CHECK(strstr(result->output, parentBefore) != NULL);
    }
    CHECK(UmiProcessSearchPathCapture(directory, &parentAfter) == UMI_STATUS_OK);
    CHECK(strcmp(parentBefore, parentAfter) == 0);
    UmiProcessSearchPathFree(parentBefore);
    UmiProcessSearchPathFree(parentAfter);
    free(result);
    return 0;
}
