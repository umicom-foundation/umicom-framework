/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/line_edit_commands.inc
 * PURPOSE: Make line edits available to shared command search and automation without duplicating document ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/line_edit.h"
static UmiStatus LineEditCommand(void *context, const char *argument, char *message, size_t capacity,
                                 UmiEditorEditCommandKind kind)
{
    /* A command always reads the current active draft. External arguments
     * cannot supply stale selection offsets or an unrelated file path. */
    UmiStatus status = argument != NULL && argument[0] != '\0'
                           ? UMI_STATUS_INVALID_ARGUMENT
                           : UmiDocumentCoordinatorEditLines(context, kind, NULL);
    if (status == UMI_STATUS_NOT_FOUND && message != NULL && capacity != 0U)
        (void)snprintf(message, capacity,
                       "Open a draft and place the caret on a line with the required neighbor.");
    else
        command_message(message, capacity,
                        "Line command completed; use Undo to restore text and Save to write the file",
                        status);
    return status;
}
static UmiStatus DeleteLineCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity, UMI_EDITOR_EDIT_COMMAND_DELETE_LINE);
}
static UmiStatus DuplicateLineCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity, UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE);
}
static UmiStatus MoveLineUpCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity, UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP);
}
static UmiStatus MoveLineDownCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity, UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN);
}
static UmiStatus JoinLineCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity, UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT);
}
static UmiStatus TrimLinesCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity,
                           UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE);
}
static UmiStatus IndentLinesCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity, UMI_EDITOR_EDIT_COMMAND_INDENT_LINES);
}
static UmiStatus OutdentLinesCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity, UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES);
}
static UmiStatus CommentLinesCommand(void *context, const char *argument, char *message, size_t capacity)
{
    return LineEditCommand(context, argument, message, capacity, UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT);
}

/* Native frontends share exact canonical identifiers with command search.
 * Unknown names leave the caller value unchanged and cannot select a default
 * destructive operation by prefix or spelling similarity. */
UmiStatus UmiDocumentLineEditCommandFromId(const char *commandId, UmiEditorEditCommandKind *outKind)
{
    if (commandId == NULL || outKind == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    static const struct { const char *id; UmiEditorEditCommandKind kind; } commands[] = {
        {UMI_DOCUMENT_COMMAND_DELETE_LINE, UMI_EDITOR_EDIT_COMMAND_DELETE_LINE},
        {UMI_DOCUMENT_COMMAND_DUPLICATE_LINE, UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE},
        {UMI_DOCUMENT_COMMAND_MOVE_LINE_UP, UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP},
        {UMI_DOCUMENT_COMMAND_MOVE_LINE_DOWN, UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN},
        {UMI_DOCUMENT_COMMAND_JOIN_LINE_WITH_NEXT, UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT},
        {UMI_DOCUMENT_COMMAND_TRIM_TRAILING_WHITESPACE, UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE},
        {UMI_DOCUMENT_COMMAND_INDENT_LINES, UMI_EDITOR_EDIT_COMMAND_INDENT_LINES},
        {UMI_DOCUMENT_COMMAND_OUTDENT_LINES, UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES},
        {UMI_DOCUMENT_COMMAND_TOGGLE_LINE_COMMENT, UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT}
    };
    for (size_t i = 0U; i < sizeof commands / sizeof commands[0]; ++i) {
        if (strcmp(commandId, commands[i].id) == 0) { *outKind = commands[i].kind; return UMI_STATUS_OK; }
    }
    return UMI_STATUS_NOT_FOUND;
}
