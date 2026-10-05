/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/document_text_batch.inc
 * PURPOSE: Reserve every view before the authoritative document transaction is published.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "document_text_batch_internal.h"

struct UmiUiDocumentTextBatch
{
    UmiUiDocumentViewModel *model;
    const UmiUiDocumentTextBatchItem *items;
    size_t count;
    size_t indices[UMI_UI_DOCUMENT_VIEW_MAX];
};

UmiStatus UmiUiDocumentTextBatchAcquire(UmiUiDocumentViewModel *model, uint64_t expected_model_revision,
                                        const UmiUiDocumentTextBatchItem *items, size_t count,
                                        UmiUiDocumentTextBatch **out_batch)
{
    if (out_batch == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_batch = NULL;
    if (model == NULL || items == NULL || count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_UI_DOCUMENT_VIEW_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiUiDocumentTextBatch *batch = calloc(1U, sizeof(*batch));
    if (batch == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    batch->model = model;
    batch->items = items;
    batch->count = count;
    (void)umi_mutex_lock(model->mutex);
    UmiStatus status = UMI_STATUS_OK;
    if (model->revision != expected_model_revision)
        status = UMI_STATUS_INVALID_STATE;
    else if (model->revision == UINT64_MAX)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        const UmiUiDocumentTextBatchItem *item = &items[i];
        if (!DocumentViewTextValid(item->after) || !umi_ui_id_is_valid(item->after->view_id) ||
            item->before_text == NULL || item->after_text == NULL)
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        if (item->after_bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES ||
            item->before_bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if (memchr(item->after_text, '\0', item->after_bytes) != NULL ||
            item->after->cursor_offset > item->after_bytes ||
            item->after->selection_length > item->after_bytes - item->after->cursor_offset)
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        size_t index = find_item(model, item->after->view_id);
        batch->indices[i] = index;
        if (index == SIZE_MAX)
        {
            status = UMI_STATUS_NOT_FOUND;
            break;
        }
        for (size_t prior = 0U; prior < i; ++prior)
            if (batch->indices[prior] == index)
            {
                status = UMI_STATUS_ALREADY_EXISTS;
                break;
            }
        if (status != UMI_STATUS_OK)
            break;
        if (model->items[index].read_only || item->after->read_only)
        {
            status = UMI_STATUS_PERMISSION_DENIED;
            break;
        }
        if (strcmp(model->items[index].document_id, item->after->document_id) != 0 ||
            strcmp(model->items[index].uri, item->after->uri) != 0 ||
            model->content[index].revision != item->expected_text_revision ||
            model->content[index].length != item->before_bytes ||
            memcmp(ContentBytes(model, index), item->before_text, item->before_bytes) != 0)
        {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        int changed = item->before_bytes != item->after_bytes ||
                      memcmp(item->before_text, item->after_text, item->before_bytes) != 0;
        if (changed && model->content[index].revision == UINT64_MAX)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Capacity changes do not publish text or advance revisions. No model API
     * is re-entered here: ReserveContent expects this existing lock. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
        status = ReserveContent(model, batch->indices[i], items[i].after_bytes);
    if (status != UMI_STATUS_OK)
    {
        UmiUiDocumentTextBatchAbort(batch);
        return status;
    }
    *out_batch = batch;
    return UMI_STATUS_OK;
}

void UmiUiDocumentTextBatchAbort(UmiUiDocumentTextBatch *batch)
{
    if (batch == NULL)
        return;
    (void)umi_mutex_unlock(batch->model->mutex);
    free(batch);
}

/* Every byte buffer and presentation field has been checked by Acquire.
 * The caller commits the authoritative store first, then transfers the same
 * complete text here. Observers cannot see half of the view set. */
void UmiUiDocumentTextBatchPublish(UmiUiDocumentTextBatch *batch)
{
    if (batch == NULL)
        return;
    UmiUiDocumentViewModel *model = batch->model;
    for (size_t i = 0U; i < batch->count; ++i)
    {
        size_t index = batch->indices[i];
        const UmiUiDocumentTextBatchItem *item = &batch->items[i];
        int changed = item->before_bytes != item->after_bytes ||
                      memcmp(item->before_text, item->after_text, item->before_bytes) != 0;
        if (model->content[index].bytes != NULL)
        {
            memcpy(model->content[index].bytes, item->after_text, item->after_bytes);
            model->content[index].bytes[item->after_bytes] = '\0';
        }
        model->items[index] = *item->after;
        SetContentPreview(&model->items[index], item->after_text, item->after_bytes);
        model->content[index].length = item->after_bytes;
        if (changed)
            ++model->content[index].revision;
    }
    ++model->revision;
    UmiUiDocumentTextBatchAbort(batch);
}
