/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/native_attach/test_plan.c
 * PURPOSE: Check immutable attachment plans and rejected paths without launching a debugger.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/native_attach.h"
#include "umicom/platform/path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            failed = 1;                                                                            \
            goto done;                                                                             \
        }                                                                                          \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    int failed = 0;
    UmiDebugNativeAttachPlan *plan = malloc(sizeof *plan), *before = malloc(sizeof *before);
    if (plan == NULL || before == NULL)
    {
        free(plan);
        free(before);
        return 1;
    }
    memset(plan, 0x5a, sizeof *plan);
    memcpy(before, plan, sizeof *plan);
    char directory[UMI_PATH_CAPACITY], long_text[2048];
    memset(long_text, 'x', sizeof long_text - 1U);
    long_text[sizeof long_text - 1U] = '\0';
    CHECK(umi_path_parent(argv[2], directory, sizeof directory) == UMI_STATUS_OK);
    UmiDebugNativeAttachOptions options = {"gdb", argv[2], "", directory, "", 12345U};
    UmiStatus expected = UMI_STATUS_OK;
    const char *mode = argv[1];
    if (strcmp(mode, "gdb") == 0)
    {
    }
    else if (strcmp(mode, "lldb") == 0)
        options.kind = "lldb";
    else if (strcmp(mode, "symbols") == 0)
        options.program = argv[2];
    else if (strcmp(mode, "builtin") == 0)
        options.executable = NULL;
    else if (strcmp(mode, "zero") == 0)
    {
        options.process_id = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "overflow") == 0)
    {
        options.process_id = UINT64_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "kind") == 0)
    {
        options.kind = "unknown";
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    else if (strcmp(mode, "relative-root") == 0)
    {
        options.working_directory = "project";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "relative-program") == 0)
    {
        options.program = "program";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "relative-adapter") == 0)
    {
        options.executable = "adapter";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "relative-tools") == 0)
    {
        options.tool_directory = "tools";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(mode, "directory-program") == 0)
    {
        options.program = directory;
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "file-root") == 0)
    {
        options.working_directory = argv[2];
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(mode, "long-root") == 0)
    {
        options.working_directory = long_text;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(mode, "utf8") == 0)
    {
        options.program = "\xff";
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else
    {
        failed = 2;
        goto done;
    }
    CHECK(UmiDebugNativeAttachPlanCreate(&options, plan) == expected);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(plan, before, sizeof *plan) == 0);
    else
    {
        CHECK(plan->process_id == 12345U && strcmp(plan->configuration.id, "native.attach") == 0);
        CHECK(strcmp(plan->configuration.working_directory, directory) == 0);
        CHECK(strstr(plan->arguments, "\"pid\":12345") != NULL);
        CHECK(strstr(plan->arguments, "\"args\"") == NULL &&
              strstr(plan->arguments, "\"env\"") == NULL);
        CHECK(strstr(plan->arguments, "\"target\"") == NULL &&
              strstr(plan->arguments, "attachCommands") == NULL);
        CHECK((strstr(plan->arguments, "\"program\"") != NULL) == (strcmp(mode, "symbols") == 0));
        CHECK(strcmp(plan->configuration.adapter, options.kind) == 0);
    }
done:
    free(plan);
    free(before);
    return failed;
}
