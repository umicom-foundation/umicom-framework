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
