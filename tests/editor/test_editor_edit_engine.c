/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_edit_engine.c
 *
 * PURPOSE:
 *   Implement the test editor edit engine behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework atomic editing engine and command transaction tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/editor/edit_engine.h"

/*
 * Exercise expect text and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void expect_text(UmiEditorEditEngine *engine, const char *expected)
{
    UmiEditorTextBufferView view;
    assert(umi_editor_edit_engine_view(engine, &view) == UMI_STATUS_OK);
    assert(view.byte_count == strlen(expected));
    assert(memcmp(view.bytes, expected, view.byte_count) == 0);
}

/*
 * Exercise command and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorEditCommandRequest command(UmiEditorEditCommandKind kind,
                                           size_t cursor)
{
    UmiEditorEditCommandRequest request;
    assert(umi_editor_edit_command_request_initialize(&request,
                                                       kind,
                                                       cursor) == UMI_STATUS_OK);
    return request;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorEditEngine *engine = NULL;
    UmiEditorEditEngineSnapshot snapshot;
    UmiEditorEditCommandRequest request;
    UmiEditorEditCommandResult result;
    const char *source = "one\ntwo\nthree";

    assert(umi_editor_edit_engine_create(NULL, &engine) == UMI_STATUS_OK);
    assert(umi_editor_edit_engine_load(engine, source, strlen(source)) ==
           UMI_STATUS_OK);

    request = command(UMI_EDITOR_EDIT_COMMAND_DELETE_LINE, 4U);
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    assert(result.changed);
    expect_text(engine, "one\nthree");
    assert(umi_editor_edit_engine_undo(engine, &result) == UMI_STATUS_OK);
    expect_text(engine, source);
    assert(umi_editor_edit_engine_redo(engine, &result) == UMI_STATUS_OK);
    expect_text(engine, "one\nthree");

    assert(umi_editor_edit_engine_load(engine, source, strlen(source)) ==
           UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE, 4U);
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "one\ntwo\ntwo\nthree");

    assert(umi_editor_edit_engine_load(engine, source, strlen(source)) ==
           UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN, 4U);
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "one\nthree\ntwo");

    assert(umi_editor_edit_engine_load(engine, source, strlen(source)) ==
           UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT, 0U);
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "one two\nthree");

    assert(umi_editor_edit_engine_load(engine,
                                        "one  \n\ttwo\t\n",
                                        strlen("one  \n\ttwo\t\n")) ==
           UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE, 0U);
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "one\n\ttwo\n");

    assert(umi_editor_edit_engine_load(engine,
                                        "  one\n  two\n",
                                        strlen("  one\n  two\n")) ==
           UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT, 0U);
    request.selection_end = strlen("  one\n  two\n");
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "  // one\n  // two\n");
    request.selection_end = strlen("  // one\n  // two\n");
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "  one\n  two\n");

    assert(umi_editor_edit_engine_load(engine, "one\ntwo", 7U) ==
           UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE, 5U);
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "one\ntwo\ntwo");

    assert(umi_editor_edit_engine_load(engine, "one\ntwo\n", 8U) ==
           UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_INDENT_LINES, 0U);
    request.selection_end = 8U;
    request.indent_text = "  ";
    request.indent_byte_count = 2U;
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "  one\n  two\n");
    request = command(UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES, 0U);
    request.selection_end = strlen("  one\n  two\n");
    request.indent_text = "  ";
    request.indent_byte_count = 2U;
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "one\ntwo\n");

    assert(umi_editor_edit_engine_load(engine, "alpha beta", 10U) ==
           UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE, 0U);
    request.selection_start = 6U;
    request.selection_end = 10U;
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "alpha BETA");
    assert(umi_editor_edit_engine_snapshot(engine, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.undo_count == 1U);
    assert(snapshot.redo_count == 0U);
    assert(snapshot.line_count == 1U);
    assert(snapshot.dirty);

    /* A new command after undo creates a new branch and removes old redo. */
    assert(umi_editor_edit_engine_undo(engine, &result) == UMI_STATUS_OK);
    request = command(UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT, 10U);
    request.text = "!";
    request.text_byte_count = 1U;
    assert(umi_editor_edit_engine_execute(engine, &request, &result) ==
           UMI_STATUS_OK);
    expect_text(engine, "alpha beta!");
    assert(umi_editor_edit_engine_redo(engine, &result) == UMI_STATUS_NOT_FOUND);

    umi_editor_edit_engine_destroy(engine);
    return 0;
}
