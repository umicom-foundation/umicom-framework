/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/watch_edit.c
 * PURPOSE: Reject stale watch edits and discard results when their expression changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "watch_edit_private.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiDebugWatchSettingsInit(UmiDebugWatchSettings *out, int enabled, const char *expression)
{
    if (out == NULL || expression == NULL || (enabled != 0 && enabled != 1) || expression[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugWatchSettings value = {0}; value.enabled = enabled;
    size_t n = 0U; while (n < sizeof value.expression && expression[n] != '\0') ++n;
    if (n == sizeof value.expression) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(value.expression, expression, n + 1U); *out = value; return UMI_STATUS_OK;
}
UmiStatus UmiDebugWatchEditCapture(UmiDebugWorkspace *workspace, size_t index, UmiDebugWatchEdit **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL; UmiDebugWatchEdit value = {0};
    UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &value.stamp);
    if (status == UMI_STATUS_OK) status = umi_debug_workspace_watch_at(workspace, index, &value.before);
    if (status != UMI_STATUS_OK) return status;
    UmiDebugWatchSettings settings;
    status = UmiDebugWatchSettingsInit(&settings, value.before.enabled, value.before.expression);
    if (status != UMI_STATUS_OK) return status;
    UmiDebugWatchEdit *edit = malloc(sizeof *edit);
    if (edit == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *edit = value; *out = edit; return UMI_STATUS_OK;
}
void UmiDebugWatchEditDestroy(UmiDebugWatchEdit *edit) { free(edit); }
UmiStatus UmiDebugWatchEditRead(const UmiDebugWatchEdit *edit, UmiDebugWatchSnapshot *out)
{
    if (edit == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = edit->before; return UMI_STATUS_OK;
}
UmiStatus UmiDebugWatchEditValidate(UmiDebugWorkspace *workspace, const UmiDebugWatchEdit *edit)
{
    if (workspace == NULL || edit == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugViewStamp now;
    UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &now);
    if (status != UMI_STATUS_OK) return status;
    return now.owner == edit->stamp.owner && now.watches == edit->stamp.watches &&
        now.sessions == edit->stamp.sessions && now.configurations == edit->stamp.configurations ?
        UMI_STATUS_OK : UMI_STATUS_BUSY;
}
UmiStatus UmiDebugWatchEditApply(UmiDebugWorkspace *workspace, const UmiDebugWatchEdit *edit,
    const UmiDebugWatchSettings *settings, UmiDebugWatchChange *out)
{
    if (out != NULL) memset(out, 0, sizeof *out);
    if (out == NULL || settings == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugWatchSettings value;
    UmiStatus status = UmiDebugWatchSettingsInit(&value, settings->enabled, settings->expression);
    if (status == UMI_STATUS_OK) status = UmiDebugWatchEditValidate(workspace, edit);
    if (status != UMI_STATUS_OK) return status;
    UmiDebugWatchChange change = {0}; change.before = change.after = edit->before;
    change.changed = value.enabled != edit->before.enabled || strcmp(value.expression, edit->before.expression) != 0;
    if (change.changed) {
        change.after.enabled = value.enabled; change.after.valid = 0;
        memset(change.after.session_id, 0, sizeof change.after.session_id);
        memset(change.after.value, 0, sizeof change.after.value);
        memset(change.after.type, 0, sizeof change.after.type);
        memcpy(change.after.expression, value.expression, sizeof value.expression);
        status = UmiDebugWorkspaceCommitWatch(workspace, &edit->before, &change.after);
        if (status != UMI_STATUS_OK) return status;
        change.after.revision = edit->stamp.watches + 1U;
    }
    *out = change; return UMI_STATUS_OK;
}
UmiStatus UmiDebugWatchEditRemove(UmiDebugWorkspace *workspace, const UmiDebugWatchEdit *edit, UmiDebugWatchChange *out)
{
    if (out != NULL) memset(out, 0, sizeof *out);
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugWatchEditValidate(workspace, edit);
    if (status == UMI_STATUS_OK) status = UmiDebugWorkspaceCommitWatch(workspace, &edit->before, NULL);
    if (status != UMI_STATUS_OK) return status;
    out->before = edit->before; out->changed = out->removed = true; return UMI_STATUS_OK;
}
