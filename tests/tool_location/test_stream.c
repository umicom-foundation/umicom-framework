/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/tool_location/test_stream.c
 * PURPOSE: Check persistent child tool selection, rejection and environment isolation through
 * native pipes. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/process_stream.h"
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
            fprintf(stderr, "%d: %s\n", __LINE__, #value);                                         \
            result = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)
static int ProgramMain(int argc, char **argv)
{
    if (argc != 4)
        return 2;
    int result = 0;
    const char *mode = argv[1], *folder = argv[2], *absoluteProgram = argv[3];
    UmiLanguageRuntimeProcessStream *stream = NULL;
    char *before = NULL, *after = NULL, *expected = NULL;
    char *output = calloc(262144U, 1U);
    CHECK(output != NULL);
    CHECK(UmiProcessSearchPathCapture(folder, &before) == UMI_STATUS_OK);
    const char *arguments[] = {folder, "literal ^&| 'argument'"};
    UmiLanguageRuntimeProcessStreamConfig config = {"umicom-tool-probe", arguments, 2U, NULL, 0};
    const char *selected = folder;
    char missing[UMI_PATH_CAPACITY];
    if (strcmp(mode, "absolute") == 0 || strcmp(mode, "inherited") == 0)
        config.program = absoluteProgram;
    if (strcmp(mode, "inherited") == 0)
        selected = NULL;
    if (strcmp(mode, "relative-folder") == 0)
        selected = "relative/tools";
    if (strcmp(mode, "relative-program") == 0)
        config.program = "relative/tool";
    if (strcmp(mode, "missing") == 0)
    {
        CHECK(umi_path_join(folder, "absent-tool-folder", missing, sizeof missing) ==
              UMI_STATUS_OK);
        CHECK(!umi_fs_exists(missing));
        selected = missing;
    }
    UmiStatus status =
        UmiLanguageRuntimeProcessStreamStartWithToolDirectory(&config, selected, &stream);
    if (strcmp(mode, "relative-folder") == 0 || strcmp(mode, "relative-program") == 0)
    {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && stream == NULL);
        goto compare_parent;
    }
    if (strcmp(mode, "missing") == 0)
    {
        CHECK(status != UMI_STATUS_OK && stream == NULL);
        goto compare_parent;
    }
    CHECK(status == UMI_STATUS_OK && stream != NULL);
    /* Read to a known output terminator, including data buffered after exit.
     * A bounded wait prevents a broken child from hanging the test process. */
    size_t used = 0U;
    for (unsigned attempt = 0U; attempt < 200U; ++attempt)
    {
        size_t count = 0U;
        CHECK(used < 262143U);
        status = umi_language_runtime_process_stream_read(stream, output + used, 262143U - used,
                                                          50U, &count);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_NOT_FOUND);
        used += count;
        output[used] = '\0';
        if (strstr(output, "\nMARKER=[") != NULL && used != 0U && output[used - 1U] == '\n')
            break;
    }
    CHECK(strstr(output, "ARG=[literal ^&| 'argument']") != NULL);
    CHECK(strstr(output, "\nMARKER=[") != NULL);
    if (selected == NULL)
    {
        /* Capture prepended folder in the parent. The inherited child sees only
         * the suffix, which may itself be empty on a minimal process host. */
        size_t offset = strlen(folder);
        if (before[offset] != '\0')
            ++offset;
        expected = malloc(strlen(before + offset) + 9U);
        CHECK(expected != NULL);
        sprintf(expected, "PATH=[%s]", before + offset);
    }
    else
    {
        expected = malloc(strlen(before) + 9U);
        CHECK(expected != NULL);
        sprintf(expected, "PATH=[%s]", before);
    }
    CHECK(strstr(output, expected) != NULL);
compare_parent:
    CHECK(UmiProcessSearchPathCapture(folder, &after) == UMI_STATUS_OK);
    CHECK(strcmp(before, after) == 0);
cleanup:
    umi_language_runtime_process_stream_destroy(stream);
    UmiProcessSearchPathFree(before);
    UmiProcessSearchPathFree(after);
    free(expected);
    free(output);
    return result;
}
#include "../native_process/utf8_entry.inc"
