/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/breakpoint_edit.c
 * PURPOSE: Validate retained breakpoint intent before publishing desired properties.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "breakpoint_edit_private.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Bound every public string before copying, including caller-built settings. */
static UmiStatus CopyProperty(char *out, size_t capacity, const char *text)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t n = 0U;
    while (n < capacity && text[n] != '\0') ++n;
    if (n == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, n + 1U); return UMI_STATUS_OK;
}
UmiStatus UmiDebugBreakpointSettingsInit(UmiDebugBreakpointSettings *out,
    int enabled, const char *condition, const char *logMessage)
{
    if (out == NULL || (enabled != 0 && enabled != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugBreakpointSettings value = {0}; value.enabled = enabled;
    UmiStatus status = CopyProperty(value.condition, sizeof value.condition, condition);
    if (status == UMI_STATUS_OK) status = CopyProperty(value.logMessage, sizeof value.logMessage, logMessage);
    if (status == UMI_STATUS_OK) *out = value;
    return status;
}
UmiStatus UmiDebugBreakpointEditCapture(UmiDebugWorkspace *workspace, size_t index, UmiDebugBreakpointEdit **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiDebugBreakpointEdit value = {0};
    UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &value.stamp);
    if (status == UMI_STATUS_OK) status = umi_debug_workspace_breakpoint_at(workspace, index, &value.before);
    if (status != UMI_STATUS_OK) return status;
    if (value.before.uri[0] == '\0' || value.before.line == 0U ||
        value.before.line > INT32_MAX || value.before.column > INT32_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugBreakpointEdit *edit = malloc(sizeof *edit);
    if (edit == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *edit = value; *out = edit; return UMI_STATUS_OK;
}
void UmiDebugBreakpointEditDestroy(UmiDebugBreakpointEdit *edit) { free(edit); }
UmiStatus UmiDebugBreakpointEditRead(const UmiDebugBreakpointEdit *edit, UmiDebugBreakpointSnapshot *out)
{
    if (edit == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = edit->before; return UMI_STATUS_OK;
}
UmiStatus UmiDebugBreakpointEditValidate(UmiDebugWorkspace *workspace, const UmiDebugBreakpointEdit *edit)
{
    if (workspace == NULL || edit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugViewStamp now;
    UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &now);
    if (status != UMI_STATUS_OK) return status;
    return now.owner == edit->stamp.owner && now.breakpoints == edit->stamp.breakpoints &&
        now.sessions == edit->stamp.sessions && now.configurations == edit->stamp.configurations ?
        UMI_STATUS_OK : UMI_STATUS_BUSY;
}
UmiStatus UmiDebugBreakpointEditApply(UmiDebugWorkspace *workspace, const UmiDebugBreakpointEdit *edit,
    const UmiDebugBreakpointSettings *settings, UmiDebugBreakpointChange *out)
{
    if (out != NULL) memset(out, 0, sizeof *out);
    if (out == NULL || settings == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugBreakpointSettings value;
    UmiStatus status = UmiDebugBreakpointSettingsInit(&value, settings->enabled, settings->condition, settings->logMessage);
    if (status == UMI_STATUS_OK) status = UmiDebugBreakpointEditValidate(workspace, edit);
    if (status != UMI_STATUS_OK) return status;
    UmiDebugBreakpointChange change = {0}; change.before = change.after = edit->before;
    change.changed = value.enabled != edit->before.enabled || strcmp(value.condition, edit->before.condition) != 0 ||
        strcmp(value.logMessage, edit->before.log_message) != 0;
    if (change.changed) {
        change.after.enabled = value.enabled; change.after.verified = 0;
        memcpy(change.after.condition, value.condition, sizeof value.condition);
        memcpy(change.after.log_message, value.logMessage, sizeof value.logMessage);
        status = UmiDebugWorkspaceCommitBreakpoint(workspace, &edit->before, &change.after);
        if (status != UMI_STATUS_OK) return status;
        /* One successful canonical upsert increments the validated generation. */
        change.after.revision = edit->stamp.breakpoints + 1U;
    }
    *out = change; return UMI_STATUS_OK;
}
UmiStatus UmiDebugBreakpointEditRemove(UmiDebugWorkspace *workspace, const UmiDebugBreakpointEdit *edit,
    UmiDebugBreakpointChange *out)
{
    if (out != NULL) memset(out, 0, sizeof *out);
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugBreakpointEditValidate(workspace, edit);
    if (status == UMI_STATUS_OK) status = UmiDebugWorkspaceCommitBreakpoint(workspace, &edit->before, NULL);
    if (status != UMI_STATUS_OK) return status;
    out->before = edit->before; out->changed = out->removed = true; return UMI_STATUS_OK;
}
