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
/* The original automatic fixture can exceed the native Windows stack before its first assertion. The replacement owns the same records on the heap; retain the earlier test and its comments for review. */
#if 0
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


#endif
#include <stdlib.h>
#include <stdio.h>

/* Large bounded records belong to this explicit fixture owner, not the native
 * thread stack. Each process still creates a fresh independent model and runs
 * the original semantic assertions; allocation failure is a test failure. */
typedef struct FixtureStorage {
    ToolTestFixture f;
    UmiAiCodingCheckpoint checkpoint;
} FixtureStorage;

static int CheckFixture(FixtureStorage *state)
{
    const char *paths[] = {"src/main.c"};

    char text[128];
    size_t length = 0U;

    assert(tool_test_fixture_init(&state->f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &state->f.workspace_storage,
        "src/main.c",
        "int value = 1;\n") == UMI_STATUS_OK);

    assert(umi_ai_coding_checkpoint_capture(
        &state->f.checkpoints,
        &state->f.workspace,
        "before",
        "Before edit",
        paths,
        1U) == UMI_STATUS_OK);

    assert(state->f.workspace.write(
        state->f.workspace.user_data,
        "src/main.c",
        "int value = 2;\n",
        strlen("int value = 2;\n")) == UMI_STATUS_OK);

    assert(umi_ai_coding_checkpoint_restore(
        &state->f.checkpoints,
        &state->f.workspace,
        "before") == UMI_STATUS_OK);

    assert(state->f.workspace.read(
        state->f.workspace.user_data,
        "src/main.c",
        text,
        sizeof(text),
        &length) == UMI_STATUS_OK);
    assert(strcmp(text, "int value = 1;\n") == 0);

    assert(umi_ai_coding_checkpoint_find(
        &state->f.checkpoints, "before", &state->checkpoint) == UMI_STATUS_OK);
    assert(state->checkpoint.file_count == 1U);

    tool_test_fixture_deinit(&state->f);
    return 0;
}

int main(void)
{
    FixtureStorage *state = calloc(1U, sizeof *state);
    if (state == NULL) {
        fputs("Cannot allocate fixture storage\n", stderr);
        return 1;
    }
    int result = CheckFixture(state);
    free(state);
    return result;
}
