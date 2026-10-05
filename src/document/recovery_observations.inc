/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/recovery_observations.inc
 * PURPOSE: Reuse existing view revisions and pending-text comparison when observing recovery candidates.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/recovery_schedule.h"
UmiStatus UmiDocumentCoordinatorObserveRecovery(UmiDocumentCoordinator *coordinator,
                                                UmiDocumentRecoveryObservation *out, size_t capacity,
                                                size_t *out_count)
{
    if (out_count == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_count = 0U;
    if (coordinator == NULL || (out == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < coordinator->count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    UmiDocumentRecoveryObservation candidate[UMI_DOCUMENT_MAX_WORKING_COPIES] = {0};
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    for (size_t i = 0U; i < coordinator->count; ++i)
    {
        UmiDocumentCoordinatorEntry *entry = &coordinator->entries[i];
        UmiDocumentEditState edit;
        UmiUiDocumentTextInfo text;
        UmiDocumentSnapshot stored;
        UmiUiDocumentViewSnapshot view;
        UmiStatus status = UmiDocumentCoordinatorGetEditState(coordinator, entry->document_id, &edit);
        if (status == UMI_STATUS_OK)
            status = UmiUiDocumentViewModelTextInfo(views, entry->view_id, &text);
        if (status == UMI_STATUS_OK)
            status = umi_document_store_snapshot(coordinator->store, entry->document_id, &stored);
        if (status == UMI_STATUS_OK)
            status = umi_ui_document_view_model_find(views, entry->view_id, &view);
        if (status != UMI_STATUS_OK)
            return status;
        char identity[UMI_UI_ID_CAPACITY];
        (void)snprintf(identity, sizeof(identity), "document.%llu", (unsigned long long)entry->document_id);
        if (strcmp(view.document_id, identity) != 0)
            return UMI_STATUS_INVALID_STATE;
        candidate[i].document_id = entry->document_id;
        candidate[i].text_revision = text.text_revision;
        candidate[i].store_revision = stored.revision;
        candidate[i].dirty = entry->edit_pending || view.dirty ||
                             (!entry->pristine_virtual && (stored.dirty || !stored.has_path));
    }
    if (coordinator->count != 0U)
        memcpy(out, candidate, coordinator->count * sizeof(*out));
    *out_count = coordinator->count;
    return UMI_STATUS_OK;
}
