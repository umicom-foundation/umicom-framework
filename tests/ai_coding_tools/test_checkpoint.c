/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_checkpoint.c
 *
 * PURPOSE:
 *   Focused behavior coverage for AI coding tools checkpoint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "tool_test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    ToolTestFixture f;
    const char *paths[] = {"src/main.c"};
    UmiAiCodingCheckpoint checkpoint;
    char text[128];
    size_t length = 0U;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &f.workspace_storage,
        "src/main.c",
        "int value = 1;\n") == UMI_STATUS_OK);

    assert(umi_ai_coding_checkpoint_capture(
        &f.checkpoints,
        &f.workspace,
        "before",
        "Before edit",
        paths,
        1U) == UMI_STATUS_OK);

    assert(f.workspace.write(
        f.workspace.user_data,
        "src/main.c",
        "int value = 2;\n",
        strlen("int value = 2;\n")) == UMI_STATUS_OK);

    assert(umi_ai_coding_checkpoint_restore(
        &f.checkpoints,
        &f.workspace,
        "before") == UMI_STATUS_OK);

    assert(f.workspace.read(
        f.workspace.user_data,
        "src/main.c",
        text,
        sizeof(text),
        &length) == UMI_STATUS_OK);
    assert(strcmp(text, "int value = 1;\n") == 0);

    assert(umi_ai_coding_checkpoint_find(
        &f.checkpoints, "before", &checkpoint) == UMI_STATUS_OK);
    assert(checkpoint.file_count == 1U);

    tool_test_fixture_deinit(&f);
    return 0;
}

