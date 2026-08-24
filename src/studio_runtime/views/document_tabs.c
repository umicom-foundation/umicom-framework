/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/document_tabs.c
 *
 * PURPOSE:
 *   Project authoritative DocumentCoordinator working copies as editor tabs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/document_tabs.h"

#include <stdio.h>

UmiStatus umi_studio_document_tabs_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    const UmiStudioRuntimeDocumentTabs *tabs;
    size_t index;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    tabs = umi_studio_runtime_platform_tabs(platform);
    if (tabs == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_studio_view_create_base(
        view_id,
        "studio-document-tabs",
        "Document Tabs",
        "Authoritative open working copies, dirty state and active editor document.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_integer(
        *out_view, "studio.tabs.count", (int64_t)tabs->count);

    for (index = 0U; status == UMI_STATUS_OK && index < tabs->count; ++index) {
        const UmiStudioRuntimeDocumentTab *tab = &tabs->tabs[index];
        char key[96];
        char row[512];

        (void)snprintf(key, sizeof(key), "studio.tabs.row.%zu", index);
        (void)snprintf(
            row, sizeof(row),
            "%s%.220s%s | %.80s | %.160s",
            tab->active ? "* " : "",
            tab->display_name,
            tab->dirty ? " •" : "",
            tab->language_id,
            tab->path);

        status = umi_studio_view_set_string(*out_view, key, row);
    }

    return status;
}
