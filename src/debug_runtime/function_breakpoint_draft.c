/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/function_breakpoint_draft.c
 * PURPOSE: Validate complete function-breakpoint edits before crossing a native process boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/function_breakpoint_session.h"
#include "umicom/editor/text_position.h"
#include <string.h>
static UmiStatus FunctionText(const char *text, size_t capacity, int required)
{
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t bytes = (size_t)(end - text);
    if (required && bytes == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    bool visible = false;
    for (size_t i = 0U; i < bytes; ++i)
        if (text[i] != ' ')
            visible = true;
    if (required && !visible)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* A control character cannot be a useful single-line name or expression in
     * this editor. Preserve interior spaces, punctuation and UTF-8 symbols. */
    for (size_t i = 0U; i < bytes; ++i)
        if ((unsigned char)text[i] < 32U || (unsigned char)text[i] == 127U)
            return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof view;
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = view.capacity = bytes;
    UmiEditorTextPosition position;
    return UmiEditorTextViewPositionAt(&view, bytes, &position);
}
UmiStatus UmiDebugFunctionDraftValidate(const UmiDebugFunctionDraft *draft)
{
    if (draft == NULL || draft->count > UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = FunctionText(draft->session_id, sizeof draft->session_id, 1);
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t i = 0U; i < draft->count; ++i)
    {
        const UmiDebugFunctionEntry *entry = &draft->entries[i];
        if (entry->enabled != 0 && entry->enabled != 1)
            return UMI_STATUS_INVALID_ARGUMENT;
        status = FunctionText(entry->breakpoint.name, sizeof entry->breakpoint.name, 1);
        if (status == UMI_STATUS_OK)
            status =
                FunctionText(entry->breakpoint.condition, sizeof entry->breakpoint.condition, 0);
        if (status == UMI_STATUS_OK)
            status = FunctionText(entry->breakpoint.hit_condition,
                                  sizeof entry->breakpoint.hit_condition, 0);
        if (status != UMI_STATUS_OK)
            return status;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(entry->breakpoint.name, draft->entries[j].breakpoint.name) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
