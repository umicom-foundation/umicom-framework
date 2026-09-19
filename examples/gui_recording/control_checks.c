/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/gui_recording/control_checks.c
 *
 * PURPOSE:
 *   Build a small Umicom Notes control map, explain inherited test targets,
 *   and detect two controls accidentally given the same identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/control_inventory.h"
#include <stdio.h>
#include <string.h>

/* The exporter borrows the stream. The caller checks and closes it. */
static UmiStatus Write(void *context, const char *bytes, size_t length)
{
    FILE *stream = context;
    return fwrite(bytes, 1U, length, stream) == length
        ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/* Each row describes one control; it is not a widget or an action handler.
 * Parent indices refer to earlier rows. The inventory copies this local row. */
static UmiStatus Add(UmiUiControlInventory *inventory, size_t parent,
    const char *id, const char *role, int interactive, int privateControl)
{
    UmiUiControlRecord record = {0};
    int idLength, roleLength;
    record.parentIndex = parent;
    record.scopeId = 1U;
    record.visible = 1;
    record.mapped = 1;
    record.enabled = 1;
    record.interactive = interactive;
    record.privateControl = privateControl;
    idLength = snprintf(record.automationId, sizeof(record.automationId), "%s", id);
    roleLength = snprintf(record.roleName, sizeof(record.roleName), "%s", role);
    if (idLength < 0 || roleLength < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)idLength >= sizeof(record.automationId) ||
        (size_t)roleLength >= sizeof(record.roleName)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiUiControlInventoryAdd(inventory, &record);
}

int main(int argc, char **argv)
{
    UmiUiControlInventory *inventory = NULL;
    UmiUiControlInventorySummary summary = {0};
    UmiUiControlRecord selected = {0};
    UmiStatus status;
    size_t editorTarget = UMI_UI_CONTROL_NO_PARENT;
    int duplicate = 0, html = 0, result = 2;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--duplicate") == 0) duplicate = 1;
        else if (strcmp(argv[i], "--html") == 0) html = 1;
        else {
            fputs("Usage: umicom-control-map-example [--duplicate] [--html]\n", stderr);
            return 2;
        }
    }
    status = UmiUiControlInventoryCreate(6U, &inventory);
    if (status != UMI_STATUS_OK) goto finish;
    /* Row 0 is the window. Its empty ID cannot conceal missing child IDs. */
    status = Add(inventory, UMI_UI_CONTROL_NO_PARENT, "", "NotesWindow", 0, 0);
    if (status == UMI_STATUS_OK) status = Add(inventory, 0U, "notes.save", "Button", 1, 0);
    if (status == UMI_STATUS_OK) status = Add(inventory, 0U, "notes.editor", "Editor", 0, 0);
    /* Row 3 is an internal editor input; its target is the tagged row 2. */
    if (status == UMI_STATUS_OK) status = Add(inventory, 2U, "", "TextInput", 1, 0);
    if (status == UMI_STATUS_OK) status = Add(inventory, 0U,
        duplicate ? "notes.save" : "notes.archive", "Button", 1, 0);
    /* A private subtree contributes only this redacted marker. */
    if (status == UMI_STATUS_OK) status = Add(inventory, 0U, "", "private-control", 1, 1);
    if (status == UMI_STATUS_OK) status = UmiUiControlInventoryTarget(inventory, 3U, &editorTarget);
    if (status == UMI_STATUS_OK && editorTarget != 2U) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiUiControlInventoryAt(inventory, editorTarget, &selected);
    if (status == UMI_STATUS_OK && (strcmp(selected.automationId, "notes.editor") != 0 ||
        UmiUiControlInventoryCount(inventory) != 6U)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiUiControlInventorySummarise(inventory, &summary);
    if (status != UMI_STATUS_OK) goto finish;
    status = html ? UmiUiControlInventoryWriteHtml(inventory, Write, stdout)
                  : UmiUiControlInventoryWriteJson(inventory, Write, stdout);
    if (fflush(stdout) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK) goto finish;
    fprintf(stderr, "%zu controls, %zu inherited targets, %zu ambiguous target records.\n",
        summary.controls, summary.inheritedTargets, summary.ambiguousTargets);
    /* A testability issue is not proof that the Save operation is broken.
     * A separate scenario must write and inspect a real Notes file. */
    result = summary.unaddressableInteractive == 0U && summary.ambiguousTargets == 0U ? 0 : 1;
finish:
    if (status != UMI_STATUS_OK) fprintf(stderr, "Control map failed: %s\n", umi_status_text(status));
    UmiUiControlInventoryDestroy(inventory);
    return result;
}
