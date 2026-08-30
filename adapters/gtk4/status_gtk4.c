/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/status_gtk4.c
 *
 * PURPOSE:
 *   Render visible Framework status items into the GTK4 application shell status row.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"

#include <stdio.h>

UmiStatus umi_gtk4_refresh_status(UmiGtk4Adapter *adapter, UmiUiWorkbench *workbench)
{
    UmiUiStatusModel *model;
    char combined[1024];
    size_t length = 0U;
    size_t index;
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    combined[0] = '\0';
    model = umi_ui_workbench_status(workbench);
    for (index = 0U; index < umi_ui_status_model_count(model); ++index) {
        UmiUiStatusSnapshot item;
        if (umi_ui_status_model_at(model, index, &item) == UMI_STATUS_OK && item.visible) {
            int written = snprintf(combined + length, sizeof(combined) - length,
                                   "%s%s", length > 0U ? " | " : "", item.text);
            if (written > 0 && (size_t)written < sizeof(combined) - length) length += (size_t)written;
        }
    }
    gtk_label_set_text(GTK_LABEL(adapter->status_label), combined);
    return UMI_STATUS_OK;
}
