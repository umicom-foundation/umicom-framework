/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/document_tabs.c
 *
 * PURPOSE:
 *   Implement DocumentCoordinator-to-editor-tab projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/document_tabs.h"

#include <string.h>

void umi_studio_document_tabs_init(
    UmiStudioRuntimeDocumentTabs *tabs)
{
    if (tabs == NULL) return;
    (void)memset(tabs, 0, sizeof(*tabs));
    tabs->revision = 1U;
}

UmiStatus umi_studio_document_tabs_refresh(
    UmiStudioRuntimeDocumentTabs *tabs,
    const UmiDocumentCoordinator *documents)
{
    size_t total;
    size_t index;

    if (tabs == NULL || documents == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    total = umi_document_coordinator_count(documents);
    if (total > UMI_STUDIO_DOCUMENT_TAB_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(tabs->tabs, 0, sizeof(tabs->tabs));
    tabs->count = 0U;
    tabs->has_active = 0;

    for (index = 0U; index < total; ++index) {
        UmiDocumentWorkingCopySnapshot source;
        UmiStudioRuntimeDocumentTab *target;

        if (umi_document_coordinator_at(
                documents,
                index,
                &source) != UMI_STATUS_OK) {
            continue;
        }

        target = &tabs->tabs[tabs->count];
        target->document_id = source.document_id;
        (void)strcpy(target->view_id, source.view_id);
        (void)strcpy(target->display_name, source.display_name);
        (void)strcpy(target->path, source.path);
        (void)strcpy(target->language_id, source.language_id);
        target->active = source.active;
        target->dirty = source.dirty;
        target->external_conflict =
            source.conflict != UMI_DOCUMENT_CONFLICT_NONE;
        target->revision = source.revision;

        if (source.active) {
            tabs->active_index = tabs->count;
            tabs->has_active = 1;
        }

        tabs->count += 1U;
    }

    tabs->revision += 1U;
    return UMI_STATUS_OK;
}
