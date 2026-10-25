/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/line_edit_commands.inc
 * PURPOSE: Make line edits available to shared command search and automation without duplicating document ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/line_edit.h"
#include "umicom/document/indentation.h"
static UmiStatus LineEditCommand(void *context, const char *argument, char *message, size_t capacity,
                                 UmiEditorEditCommandKind kind)
{
    /* A command always reads the current active draft. External arguments
     * cannot supply stale selection offsets or an unrelated file path. */
    UmiStatus status = argument != NULL && argument[0] != '\0'
                           ? UMI_STATUS_INVALID_ARGUMENT
                           : UmiDocumentCoordinatorEditLines(context, kind, NULL);
    if (status == UMI_STATUS_NOT_FOUND && message != NULL && capacity != 0U)
        /* Selection commands explain their missing input without implying a
         * neighbor is required. The previous caret-only message remains below. */
#if 0
        (void)snprintf(message, capacity,
                       "Open a draft and place the caret on a line with the required neighbor.");
#endif
        (void)snprintf(message, capacity, "%s",
            kind == UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE ||
            kind == UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE
                ? "Select text to convert its ASCII letters, then run the command again."
                : kind == UMI_EDITOR_EDIT_COMMAND_SORT_LINES_ASCENDING ||
            kind == UMI_EDITOR_EDIT_COMMAND_SORT_LINES_DESCENDING
                ? "Select the source lines to sort, then run the command again."
                : "Open a draft and place the caret on a line with the required neighbor.");
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

/* Both entry points read the current draft through the same document owner. */
static UmiStatus UppercaseSelectionCommand(void *context,const char *argument,char *message,size_t capacity)
{
    return LineEditCommand(context,argument,message,capacity,UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE);
}
static UmiStatus LowercaseSelectionCommand(void *context,const char *argument,char *message,size_t capacity)
{
    return LineEditCommand(context,argument,message,capacity,UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE);
}
static UmiStatus SortAscendingCommand(void *context,const char *argument,char *message,size_t capacity)
{
    return LineEditCommand(context,argument,message,capacity,UMI_EDITOR_EDIT_COMMAND_SORT_LINES_ASCENDING);
}
static UmiStatus SortDescendingCommand(void *context,const char *argument,char *message,size_t capacity)
{
    return LineEditCommand(context,argument,message,capacity,UMI_EDITOR_EDIT_COMMAND_SORT_LINES_DESCENDING);
}

/* Native frontends share exact canonical identifiers with command search.
 * Unknown names leave the caller value unchanged and cannot select a default
 * destructive operation by prefix or spelling similarity. */
UmiStatus UmiDocumentLineEditCommandFromId(const char *commandId, UmiEditorEditCommandKind *outKind)
{
    if (commandId == NULL || outKind == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    static const struct { const char *id; UmiEditorEditCommandKind kind; } commands[] = {
        {UMI_DOCUMENT_COMMAND_UPPERCASE_SELECTION, UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE},
        {UMI_DOCUMENT_COMMAND_LOWERCASE_SELECTION, UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE},
        {UMI_DOCUMENT_COMMAND_SORT_LINES_ASCENDING, UMI_EDITOR_EDIT_COMMAND_SORT_LINES_ASCENDING},
        {UMI_DOCUMENT_COMMAND_SORT_LINES_DESCENDING, UMI_EDITOR_EDIT_COMMAND_SORT_LINES_DESCENDING},
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

/* Headless and native clients use the same policy and document history. */
static UmiStatus InsertIndentationCommand(void *context,const char *argument,char *message,size_t capacity)
{
    UmiDocumentWorkingCopySnapshot active;
    UmiStatus status = argument != NULL && argument[0] != '\0' ? UMI_STATUS_INVALID_ARGUMENT :
        umi_document_coordinator_active_snapshot(context, &active);
    if (status == UMI_STATUS_OK) status = UmiDocumentCoordinatorInsertIndentation(context, active.document_id);
    command_message(message, capacity, "Indentation inserted; use Undo to restore the draft", status);
    return status;
}

/* Newline command search uses the same owner and selection as source keys. */
#include "umicom/document/newline.h"
static UmiStatus NewlineCommand(void *context, const char *argument, char *message,
    size_t capacity, int inherit)
{
    UmiDocumentWorkingCopySnapshot active;
    UmiStatus status = argument != NULL && argument[0] != '\0' ? UMI_STATUS_INVALID_ARGUMENT :
        umi_document_coordinator_active_snapshot(context, &active);
    if (status == UMI_STATUS_OK) status = UmiDocumentCoordinatorInsertNewline(context, active.document_id, inherit);
    command_message(message, capacity, "New line inserted; use Undo to restore the draft", status);
    return status;
}
static UmiStatus InsertNewlineCommand(void *context,const char *argument,char *message,size_t capacity)
{ return NewlineCommand(context,argument,message,capacity,1); }
static UmiStatus InsertPlainNewlineCommand(void *context,const char *argument,char *message,size_t capacity)
{ return NewlineCommand(context,argument,message,capacity,0); }

/* Command search shares the named source policy and existing Undo transaction. */
#include "umicom/document/language_comment.h"
static UmiStatus LanguageCommentCommand(void *context,const char *argument,char *message,size_t capacity)
{
    UmiDocumentWorkingCopySnapshot active;
    UmiStatus status=argument!=NULL&&argument[0]!='\0'?UMI_STATUS_INVALID_ARGUMENT:
        umi_document_coordinator_active_snapshot(context,&active);
    if(status==UMI_STATUS_OK)status=UmiDocumentCoordinatorToggleLanguageComment(context,active.document_id);
    command_message(message,capacity,"Language line comments toggled; inspect the draft and use Undo if needed",status);
    return status;
}
