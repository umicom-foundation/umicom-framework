/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/document_identity.inc
 *
 * PURPOSE:
 *   Publish checked document identities as one metadata transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Included by document_view.c to share its lock and owned content. Identity
 * adoption must never pass a bounded preview through the full-text setter. */
UmiStatus UmiUiDocumentViewModelBindIdentities(UmiUiDocumentViewModel *model,
    uint64_t expectedRevision, const UmiUiDocumentIdentityBinding *bindings, size_t count)
{
    if (model == NULL || (count != 0U && bindings == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_UI_DOCUMENT_VIEW_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        const UmiUiDocumentIdentityBinding *row = &bindings[i];
        if (memchr(row->view_id, '\0', sizeof row->view_id) == NULL ||
            memchr(row->expected_document_id, '\0', sizeof row->expected_document_id) == NULL ||
            memchr(row->document_id, '\0', sizeof row->document_id) == NULL ||
            !umi_ui_id_is_valid(row->view_id) || !umi_ui_id_is_valid(row->document_id))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t previous = 0U; previous < i; ++previous)
            if (strcmp(row->view_id, bindings[previous].view_id) == 0)
                return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiStatus status = umi_mutex_lock(model->mutex);
    if (status != UMI_STATUS_OK) return status;
    size_t indices[UMI_UI_DOCUMENT_VIEW_MAX];
    if (model->revision != expectedRevision) status = UMI_STATUS_BUSY;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i) {
        indices[i] = find_item(model, bindings[i].view_id);
        if (indices[i] == SIZE_MAX) status = UMI_STATUS_NOT_FOUND;
        else if (strcmp(model->items[indices[i]].document_id,
                         bindings[i].expected_document_id) != 0)
            status = UMI_STATUS_BUSY;
    }
    if (status == UMI_STATUS_OK) {
        int changed = 0;
        for (size_t i = 0U; i < count; ++i) {
            size_t index = indices[i];
            if (strcmp(model->items[index].document_id, bindings[i].document_id) == 0) continue;
            strcpy(model->items[index].document_id, bindings[i].document_id);
            model->content[index].revision = umi_ui_next_revision(model->content[index].revision);
            changed = 1;
        }
        if (changed) model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}
