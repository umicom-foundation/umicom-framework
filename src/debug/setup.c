/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/setup.c
 * PURPOSE: Own reusable debug setup capture and reviewed replacement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "setup_private.h"
#include "umicom/document/text_encoding.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
/* Bound arrays before examining UTF-8. Conditions may contain tabs/newlines;
 * source locations and titles are single-line text for unambiguous review. */
static UmiStatus SetupText(const char *text, size_t capacity, int required, int singleLine)
{
    if (text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t n = 0U;
    while (n < capacity && text[n] != '\0')
        ++n;
    if (n == capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if ((required && n == 0U) || !umi_document_utf8_validate((const unsigned char *)text, n, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < n; ++i)
    {
        unsigned char c = (unsigned char)text[i];
        if (c == 127U || (c < 32U && (singleLine || (c != '\n' && c != '\r' && c != '\t'))))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugSetupCreate(const char *title, UmiDebugSetup **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = SetupText(title, 256U, 1, 1);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDebugSetup *setup = calloc(1U, sizeof *setup);
    if (setup == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(setup->summary.title, title, strlen(title) + 1U);
    *out = setup;
    return UMI_STATUS_OK;
}
void UmiDebugSetupDestroy(UmiDebugSetup *setup) { free(setup); }
UmiStatus UmiDebugSetupCopy(const UmiDebugSetup *setup, UmiDebugSetup **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (setup == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugSetup *copy = malloc(sizeof *copy);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    *copy = *setup;
    *out = copy;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugSetupInspect(const UmiDebugSetup *setup, UmiDebugSetupSummary *out)
{
    if (setup == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = setup->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugSetupAddBreakpoint(UmiDebugSetup *setup, const UmiDebugSetupBreakpoint *value)
{
    if (setup == NULL || value == NULL || value->line == 0U || value->line > INT32_MAX ||
        value->column > INT32_MAX || (value->enabled != 0 && value->enabled != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = SetupText(value->source, sizeof value->source, 1, 1);
    if (status == UMI_STATUS_OK)
        status = SetupText(value->condition, sizeof value->condition, 0, 0);
    if (status == UMI_STATUS_OK)
        status = SetupText(value->logMessage, sizeof value->logMessage, 0, 0);
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t i = 0U; i < setup->summary.breakpoints; ++i)
    {
        const UmiDebugSetupBreakpoint *old = &setup->breakpoints[i];
        if (old->line == value->line && old->column == value->column &&
            strcmp(old->source, value->source) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    if (setup->summary.breakpoints == UMI_DEBUG_SETUP_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    setup->breakpoints[setup->summary.breakpoints++] = *value;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugSetupAddWatch(UmiDebugSetup *setup, const UmiDebugSetupWatch *value)
{
    if (setup == NULL || value == NULL || (value->enabled != 0 && value->enabled != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = SetupText(value->expression, sizeof value->expression, 1, 0);
    if (status != UMI_STATUS_OK)
        return status;
    if (setup->summary.watches == UMI_DEBUG_SETUP_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    setup->watches[setup->summary.watches++] = *value;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugSetupBreakpointAt(const UmiDebugSetup *setup, size_t index, UmiDebugSetupBreakpoint *out)
{
    if (setup == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= setup->summary.breakpoints)
        return UMI_STATUS_NOT_FOUND;
    *out = setup->breakpoints[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugSetupWatchAt(const UmiDebugSetup *setup, size_t index, UmiDebugSetupWatch *out)
{
    if (setup == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= setup->summary.watches)
        return UMI_STATUS_NOT_FOUND;
    *out = setup->watches[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugSetupCapture(UmiDebugWorkspace *workspace, const char *title, UmiDebugSetup **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiDebugWorkspaceSnapshot snapshot;
    UmiStatus status = umi_debug_workspace_snapshot(workspace, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    if (snapshot.breakpoint_count > UMI_DEBUG_SETUP_CAPACITY ||
        snapshot.watch_count > UMI_DEBUG_SETUP_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDebugSetup *setup = NULL;
    status = UmiDebugSetupCreate(title, &setup);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot.breakpoint_count; ++i)
    {
        UmiDebugBreakpointSnapshot item;
        status = umi_debug_workspace_breakpoint_at(workspace, i, &item);
        if (status != UMI_STATUS_OK)
            break;
        UmiDebugSetupBreakpoint value = {0};
        memcpy(value.source, item.uri, sizeof value.source);
        memcpy(value.condition, item.condition, sizeof value.condition);
        memcpy(value.logMessage, item.log_message, sizeof value.logMessage);
        value.line = item.line;
        value.column = item.column;
        value.enabled = item.enabled;
        status = UmiDebugSetupAddBreakpoint(setup, &value);
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot.watch_count; ++i)
    {
        UmiDebugWatchSnapshot item;
        status = umi_debug_workspace_watch_at(workspace, i, &item);
        if (status != UMI_STATUS_OK)
            break;
        UmiDebugSetupWatch value = {0};
        memcpy(value.expression, item.expression, sizeof value.expression);
        value.enabled = item.enabled;
        status = UmiDebugSetupAddWatch(setup, &value);
    }
    if (status == UMI_STATUS_OK)
        *out = setup;
    else
        UmiDebugSetupDestroy(setup);
    return status;
}
UmiStatus UmiDebugSetupReviewCreate(UmiDebugWorkspace *workspace, const UmiDebugSetup *proposed,
                                    UmiDebugSetupReview **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (workspace == NULL || proposed == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugSetupReview *review = calloc(1U, sizeof *review);
    if (review == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiDebugSetupCapture(workspace, "Current debugger settings", &review->before);
    if (status == UMI_STATUS_OK)
        status = UmiDebugSetupCopy(proposed, &review->after);
    if (status == UMI_STATUS_OK)
        status = UmiDebugWorkspaceViewStamp(workspace, &review->stamp);
    if (status == UMI_STATUS_OK)
        *out = review;
    else
        UmiDebugSetupReviewDestroy(review);
    return status;
}
void UmiDebugSetupReviewDestroy(UmiDebugSetupReview *review)
{
    if (review != NULL)
    {
        UmiDebugSetupDestroy(review->before);
        UmiDebugSetupDestroy(review->after);
        free(review);
    }
}
const UmiDebugSetup *UmiDebugSetupReviewBefore(const UmiDebugSetupReview *review)
{
    return review != NULL ? review->before : NULL;
}
const UmiDebugSetup *UmiDebugSetupReviewAfter(const UmiDebugSetupReview *review)
{
    return review != NULL ? review->after : NULL;
}
UmiStatus UmiDebugSetupReviewValidate(UmiDebugWorkspace *workspace, const UmiDebugSetupReview *review)
{
    if (workspace == NULL || review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugViewStamp now;
    UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &now);
    if (status != UMI_STATUS_OK)
        return status;
    const UmiDebugViewStamp *old = &review->stamp;
    if (now.owner != old->owner || now.controller != old->controller || now.sessions != old->sessions ||
        now.configurations != old->configurations || now.breakpoints != old->breakpoints ||
        now.watches != old->watches)
        return UMI_STATUS_BUSY;
    UmiDebugWorkspaceSnapshot state;
    status = umi_debug_workspace_snapshot(workspace, &state);
    if (status != UMI_STATUS_OK)
        return status;
    return state.controller_state == UMI_DEBUG_CONTROLLER_IDLE ||
                   state.controller_state == UMI_DEBUG_CONTROLLER_TERMINATED ||
                   state.controller_state == UMI_DEBUG_CONTROLLER_FAILED
               ? UMI_STATUS_OK
               : UMI_STATUS_BUSY;
}
UmiStatus UmiDebugSetupReviewApply(UmiDebugWorkspace *workspace, const UmiDebugSetupReview *review)
{
    UmiStatus status = UmiDebugSetupReviewValidate(workspace, review);
    return status == UMI_STATUS_OK ? UmiDebugWorkspaceCommitSetup(workspace, review->after) : status;
}
