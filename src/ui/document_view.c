/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/document_view.c
 *
 * PURPOSE:
 *   Implement the toolkit-neutral document-tab model including preview,
 *   grouping, navigation, activation, close safety and pinning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/document_view.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "ui_internal.h"

/* One owned draft accompanies each tab. Small drafts use the existing inline
 * snapshot; larger drafts allocate only when needed. No editor algorithm or
 * disk persistence is implemented here: this remains a presentation model. */
typedef struct UmiUiDocumentContent {
    char *bytes;
    size_t capacity;
    size_t length;
    uint64_t revision;
} UmiUiDocumentContent;

struct UmiUiDocumentViewModel {
    UmiUiDocumentViewSnapshot items[UMI_UI_DOCUMENT_VIEW_MAX];
    UmiUiDocumentContent content[UMI_UI_DOCUMENT_VIEW_MAX];
    size_t allocated_bytes;
    size_t count;
    uint64_t revision;
    UmiMutex *mutex;
};

/* Validate fixed-size presentation fields before copying them into the model.
 * A rejected update must not replace a valid tab or advance its revision. */
static int DocumentViewTextValid(const UmiUiDocumentViewSnapshot *item)
{
    return item != NULL &&
        memchr(item->view_id, '\0', sizeof item->view_id) != NULL &&
        memchr(item->document_id, '\0', sizeof item->document_id) != NULL &&
        memchr(item->title, '\0', sizeof item->title) != NULL &&
        memchr(item->icon_name, '\0', sizeof item->icon_name) != NULL &&
        memchr(item->uri, '\0', sizeof item->uri) != NULL &&
        memchr(item->language_id, '\0', sizeof item->language_id) != NULL &&
        memchr(item->source_text, '\0', sizeof item->source_text) != NULL &&
        memchr(item->group_id, '\0', sizeof item->group_id) != NULL;
}

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiUiDocumentViewModel *model, const char *id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].view_id, id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the effective group operation used by this module and its client applications. */
static const char *effective_group(const UmiUiDocumentViewSnapshot *item)
{
    return item->group_id[0] != '\0'
        ? item->group_id
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
}

/* Provide the same group operation used by this module and its client applications. */
static int same_group(const UmiUiDocumentViewSnapshot *left,
                      const UmiUiDocumentViewSnapshot *right)
{
    return strcmp(effective_group(left), effective_group(right)) == 0;
}

/* Find erase while leaving the underlying catalogue or model owned by this module. */
static void erase_at(UmiUiDocumentViewModel *model, size_t index)
{
    model->allocated_bytes -= model->content[index].capacity;
    free(model->content[index].bytes);
    if (index + 1U < model->count) {
        memmove(&model->items[index], &model->items[index + 1U],
            (model->count - index - 1U) * sizeof(model->items[0]));
        memmove(&model->content[index], &model->content[index + 1U],
            (model->count - index - 1U) * sizeof(model->content[0]));
    }
    --model->count;
    memset(&model->items[model->count], 0, sizeof(model->items[0]));
    memset(&model->content[model->count], 0, sizeof(model->content[0]));
}

/* Inserting a tab creates an empty ownership slot. A move transfers the old
 * slot back after insertion; it must not free the draft it is moving. */
static void insert_at(UmiUiDocumentViewModel *model, size_t index,
    const UmiUiDocumentViewSnapshot *item)
{
    if (index < model->count) {
        memmove(&model->items[index + 1U], &model->items[index],
            (model->count - index) * sizeof(model->items[0]));
        memmove(&model->content[index + 1U], &model->content[index],
            (model->count - index) * sizeof(model->content[0]));
    }
    model->items[index] = *item;
    memset(&model->content[index], 0, sizeof(model->content[0]));
    model->content[index].length = strlen(item->source_text);
    model->content[index].revision = 1U;
    ++model->count;
}

/* Read only while holding the model mutex. */
static const char *ContentBytes(const UmiUiDocumentViewModel *model, size_t index)
{
    return model->content[index].bytes != NULL
        ? model->content[index].bytes : model->items[index].source_text;
}

/* Capacity is an implementation detail; reserving does not report an edit.
 * Growth is amortised and both additions are bounded before allocation. */
static UmiStatus ReserveContent(UmiUiDocumentViewModel *model, size_t index,
    size_t length)
{
    UmiUiDocumentContent *content = &model->content[index];
    if (length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length < UMI_UI_DOCUMENT_CONTENT_CAPACITY && content->bytes == NULL)
        return UMI_STATUS_OK;
    size_t required = length + 1U;
    if (required <= content->capacity) return UMI_STATUS_OK;
    size_t capacity = content->capacity != 0U
        ? content->capacity : UMI_UI_DOCUMENT_CONTENT_CAPACITY;
    while (capacity < required) {
        if (capacity > (UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U) / 2U) {
            capacity = UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES + 1U;
            break;
        }
        capacity *= 2U;
    }
    size_t growth = capacity - content->capacity;
    if (growth > UMI_UI_DOCUMENT_TEXT_BUDGET_BYTES - model->allocated_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    int wasInline = content->bytes == NULL;
    char *replacement = realloc(content->bytes, capacity);
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (wasInline)
        memcpy(replacement, model->items[index].source_text, content->length + 1U);
    content->bytes = replacement;
    content->capacity = capacity;
    model->allocated_bytes += growth;
    return UMI_STATUS_OK;
}

/* Keep the legacy snapshot readable without cutting a UTF-8 sequence in half.
 * Full content is always read from its owned storage, never from this preview. */
static void SetContentPreview(UmiUiDocumentViewSnapshot *item,
    const char *text, size_t length)
{
    size_t preview = length < sizeof item->source_text
        ? length : sizeof item->source_text - 1U;
    if (preview < length) {
        while (preview > 0U && ((unsigned char)text[preview] & 0xc0U) == 0x80U)
            --preview;
    }
    memmove(item->source_text, text, preview);
    item->source_text[preview] = '\0';
}

/* Provide the resequence orders operation used by this module and its client applications. */
static void resequence_orders(UmiUiDocumentViewModel *model)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        size_t previous;
        int32_t order = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (previous = 0U; previous < index; ++previous) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (same_group(&model->items[previous], &model->items[index])) {
                order += 1;
            }
        }
        model->items[index].order = order;
    }
}

/* Provide the copy identifier operation used by this module and its client applications. */
static UmiStatus copy_identifier(char *destination,
                                 size_t capacity,
                                 const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ui document view model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_document_view_model_create(UmiUiDocumentViewModel **out_model)
{
    UmiUiDocumentViewModel *model;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiUiDocumentViewModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&model->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(model);
        return status;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ui document view model so the same storage can be reused
 * safely.
 */
void umi_ui_document_view_model_destroy(UmiUiDocumentViewModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    for (size_t index = 0U; index < model->count; ++index)
        free(model->content[index].bytes);
    umi_mutex_destroy(model->mutex);
    free(model);
}

/*
 * Provide the ui document view model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_view_model_upsert(
    UmiUiDocumentViewModel *model, const UmiUiDocumentViewSnapshot *item)
{
    if (model == NULL || !DocumentViewTextValid(item) ||
        !umi_ui_id_is_valid(item->view_id)) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    size_t index = find_item(model, item->view_id);
    if (index != SIZE_MAX && model->content[index].length >= UMI_UI_DOCUMENT_CONTENT_CAPACITY &&
        strcmp(item->document_id, model->items[index].document_id) == 0) {
        /* Legacy metadata changes must not replace a full draft with a prefix. */
        if (strcmp(item->source_text, model->items[index].source_text) != 0) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        model->items[index] = *item;
        model->revision = umi_ui_next_revision(model->revision);
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_OK;
    }
    if (index == SIZE_MAX) {
        if (model->count >= UMI_UI_DOCUMENT_VIEW_MAX) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = model->count++;
    }
    size_t length = strlen(item->source_text);
    int changed = model->content[index].revision == 0U ||
        strcmp(model->items[index].document_id, item->document_id) != 0 ||
        model->content[index].length != length ||
        memcmp(ContentBytes(model, index), item->source_text, length) != 0;
    if (model->content[index].bytes != NULL)
        memcpy(model->content[index].bytes, item->source_text, length + 1U);
    model->items[index] = *item;
    model->content[index].length = length;
    if (changed) model->content[index].revision = umi_ui_next_revision(model->content[index].revision);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/* Full-text publication copies borrowed input only after every bound check. */
UmiStatus UmiUiDocumentViewModelUpsertText(UmiUiDocumentViewModel *model,
    const UmiUiDocumentViewSnapshot *item, const char *text, size_t length)
{
    if (model == NULL || !DocumentViewTextValid(item) ||
        !umi_ui_id_is_valid(item->view_id) || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\0', length) != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiUiDocumentViewSnapshot updated = *item;
    /* Do this before changing model storage: item may be a caller's snapshot. */
    SetContentPreview(&updated, text, length);
    (void)umi_mutex_lock(model->mutex);
    size_t index = find_item(model, item->view_id);
    int newItem = index == SIZE_MAX;
    if (newItem) {
        if (model->count >= UMI_UI_DOCUMENT_VIEW_MAX) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = model->count;
    }
    UmiStatus status = ReserveContent(model, index, length);
    if (status == UMI_STATUS_OK) {
        int changed = newItem || strcmp(model->items[index].document_id, item->document_id) != 0 ||
            model->content[index].length != length ||
            memcmp(ContentBytes(model, index), text, length) != 0;
        if (model->content[index].bytes != NULL) {
            memcpy(model->content[index].bytes, text, length);
            model->content[index].bytes[length] = '\0';
        }
        model->items[index] = updated;
        model->content[index].length = length;
        if (changed) model->content[index].revision = umi_ui_next_revision(model->content[index].revision);
        if (newItem) ++model->count;
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

/* Reserve under the same mutex used for publication. */
UmiStatus UmiUiDocumentViewModelReserveText(UmiUiDocumentViewModel *model,
    const char *viewId, size_t length)
{
    if (model == NULL || viewId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    size_t index = find_item(model, viewId);
    UmiStatus status = index == SIZE_MAX ? UMI_STATUS_NOT_FOUND
        : ReserveContent(model, index, length);
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

/* Return independent storage; no borrowed pointer escapes the model mutex. */
UmiStatus UmiUiDocumentViewModelCopyText(const UmiUiDocumentViewModel *model,
    const char *viewId, char **outText, size_t *outLength)
{
    if (model == NULL || viewId == NULL || outText == NULL || outLength == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    size_t index = find_item(model, viewId);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    size_t length = model->content[index].length;
    char *copy = malloc(length + 1U);
    if (copy == NULL) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy, ContentBytes(model, index), length);
    copy[length] = '\0';
    *outText = copy;
    *outLength = length;
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/* Pair allocation and release within the same Framework runtime. */
void UmiUiDocumentViewModelFreeText(char *text)
{
    free(text);
}

/* Navigation can inspect the length/revision without copying the document. */
UmiStatus UmiUiDocumentViewModelTextInfo(const UmiUiDocumentViewModel *model,
    const char *viewId, UmiUiDocumentTextInfo *outInfo)
{
    if (model == NULL || viewId == NULL || outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    size_t index = find_item(model, viewId);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *outInfo = (UmiUiDocumentTextInfo){model->content[index].length,
        model->content[index].revision,
        model->content[index].length < UMI_UI_DOCUMENT_CONTENT_CAPACITY};
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Remove ui document view model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_document_view_model_remove(UmiUiDocumentViewModel *model,
                                             const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    erase_at(model, index);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Find ui document view model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_document_view_model_find(
    const UmiUiDocumentViewModel *model,
    const char *id,
    UmiUiDocumentViewSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || id == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Find ui document view model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_document_view_model_at(
    const UmiUiDocumentViewModel *model,
    size_t index,
    UmiUiDocumentViewSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = model->items[index];
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ui document view model without changing
 * their state.
 */
size_t umi_ui_document_view_model_count(const UmiUiDocumentViewModel *model)
{
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        count = model->count;
        (void)umi_mutex_unlock(model->mutex);
    }
    return count;
}

/*
 * Provide the ui document view model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_document_view_model_revision(
    const UmiUiDocumentViewModel *model)
{
    uint64_t revision = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) {
        (void)umi_mutex_lock(model->mutex);
        revision = model->revision;
        (void)umi_mutex_unlock(model->mutex);
    }
    return revision;
}

/*
 * Provide the ui document view model open preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_open_preview(
    UmiUiDocumentViewModel *model,
    const UmiUiDocumentViewSnapshot *item)
{
    UmiUiDocumentViewSnapshot preview;
    size_t target;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !DocumentViewTextValid(item) ||
        !umi_ui_id_is_valid(item->view_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    preview = *item;
    preview.preview = 1;
    preview.pinned = 0;
    preview.active = 1;

    (void)umi_mutex_lock(model->mutex);
    target = find_item(model, preview.view_id);
    /* Configure the optional target only when its feature has created it. */
    if (target != SIZE_MAX) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < model->count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (same_group(&model->items[index], &model->items[target])) {
                model->items[index].active = index == target;
            }
        }
        model->revision = umi_ui_next_revision(model->revision);
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_OK;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &preview) &&
            model->items[index].preview &&
            !model->items[index].dirty &&
            !model->items[index].pinned) {
            target = index;
            break;
        }
    }
    /* Configure the optional target only when its feature has created it. */
    if (target == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->count >= UMI_UI_DOCUMENT_VIEW_MAX) {
            (void)umi_mutex_unlock(model->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        target = model->count++;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &preview)) {
            model->items[index].active = 0;
        }
    }
    model->allocated_bytes -= model->content[target].capacity;
    free(model->content[target].bytes);
    model->content[target] = (UmiUiDocumentContent){0};
    model->content[target].length = strlen(preview.source_text);
    model->content[target].revision = 1U;
    model->items[target] = preview;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model set pinned operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_set_pinned(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int pinned)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].pinned = pinned != 0;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (pinned != 0) model->items[index].preview = 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model promote preview operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_promote_preview(
    UmiUiDocumentViewModel *model,
    const char *item_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].preview = 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model set word wrap operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_set_word_wrap(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int word_wrap)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    model->items[index].word_wrap = word_wrap != 0;
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model move to group operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_move_to_group(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    const char *group_id)
{
    size_t index;
    size_t other;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || group_id == NULL ||
        !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    status = copy_identifier(model->items[index].group_id,
                             sizeof(model->items[index].group_id),
                             group_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK && model->items[index].active) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (other = 0U; other < model->count; ++other) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (other != index &&
                same_group(&model->items[other], &model->items[index])) {
                model->items[other].active = 0;
            }
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

/*
 * Provide the ui document view model place operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_view_model_place(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    const char *group_id,
    size_t position)
{
    UmiUiDocumentViewSnapshot item;
    UmiUiDocumentViewSnapshot requested = {0};
    size_t index;
    size_t insert_index;
    size_t group_position = 0U;
    size_t last_group_index = SIZE_MAX;
    size_t other;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || group_id == NULL ||
        !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(requested.group_id,
                             sizeof(requested.group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    item = model->items[index];
    UmiUiDocumentContent movingContent = model->content[index];
    memset(&model->content[index], 0, sizeof(model->content[index]));
    erase_at(model, index);
    status = copy_identifier(item.group_id, sizeof(item.group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        insert_at(model, index, &item);
        model->content[index] = movingContent;
        (void)umi_mutex_unlock(model->mutex);
        return status;
    }

    insert_index = model->count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (other = 0U; other < model->count; ++other) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!same_group(&model->items[other], &requested)) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (group_position == position) {
            insert_index = other;
            break;
        }
        last_group_index = other;
        group_position += 1U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (insert_index == model->count && last_group_index != SIZE_MAX) {
        insert_index = last_group_index + 1U;
    }
    insert_at(model, insert_index, &item);
    model->content[insert_index] = movingContent;

    /* Apply this operation only while the related capability or state is available. */
    if (item.active) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (other = 0U; other < model->count; ++other) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (other != insert_index &&
                same_group(&model->items[other], &model->items[insert_index])) {
                model->items[other].active = 0;
            }
        }
    }
    resequence_orders(model);
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_view_model_activate(
    UmiUiDocumentViewModel *model,
    const char *item_id)
{
    size_t index;
    size_t other;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    index = find_item(model, item_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (other = 0U; other < model->count; ++other) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[other], &model->items[index])) {
            model->items[other].active = other == index;
        }
    }
    model->revision = umi_ui_next_revision(model->revision);
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model activate relative operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_document_view_model_activate_relative(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    int direction,
    char *out_item_id,
    size_t capacity)
{
    size_t current;
    size_t target;
    size_t index;
    size_t visited;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || direction == 0 ||
        out_item_id == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    current = find_item(model, item_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (current == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    target = current;
    /* Visit each bounded item once so every record receives the same rule. */
    for (visited = 0U; visited < model->count; ++visited) {
        target = direction > 0
            ? (target + 1U) % model->count
            : (target == 0U ? model->count - 1U : target - 1U);
        /* Apply this branch only when its contract condition is satisfied. */
        if (same_group(&model->items[target], &model->items[current])) break;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &model->items[current])) {
            model->items[index].active = index == target;
        }
    }
    status = copy_identifier(out_item_id, capacity,
                             model->items[target].view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

/*
 * Provide the ui document view model activate group operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_activate_group(
    UmiUiDocumentViewModel *model,
    const char *group_id,
    char *out_item_id,
    size_t capacity)
{
    UmiUiDocumentViewSnapshot requested = {0};
    size_t index;
    size_t target = SIZE_MAX;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || group_id == NULL || out_item_id == NULL ||
        capacity == 0U || !umi_ui_id_is_valid(group_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(requested.group_id,
                             sizeof(requested.group_id),
                             group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &requested)) {
            /* Configure the optional target only when its feature has created it. */
            if (target == SIZE_MAX || model->items[index].active) {
                target = index;
            }
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (model->items[index].active) break;
        }
    }
    /* Configure the optional target only when its feature has created it. */
    if (target == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &requested)) {
            model->items[index].active = index == target;
        }
    }
    status = copy_identifier(out_item_id, capacity,
                             model->items[target].view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return status;
}

/*
 * Provide the ui document view model merge group operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_merge_group(
    UmiUiDocumentViewModel *model,
    const char *source_group_id,
    const char *target_group_id)
{
    UmiUiDocumentViewSnapshot source = {0};
    UmiUiDocumentViewSnapshot target = {0};
    size_t index;
    int target_has_active = 0;
    int changed = 0;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || source_group_id == NULL || target_group_id == NULL ||
        !umi_ui_id_is_valid(source_group_id) ||
        !umi_ui_id_is_valid(target_group_id) ||
        strcmp(source_group_id, target_group_id) == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_identifier(source.group_id, sizeof(source.group_id),
                             source_group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_identifier(target.group_id, sizeof(target.group_id),
                                 target_group_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(model->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &target) &&
            model->items[index].active) {
            target_has_active = 1;
            break;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &source)) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (target_has_active && model->items[index].active) {
                model->items[index].active = 0;
            } else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (model->items[index].active) {
                target_has_active = 1;
            }
            status = copy_identifier(model->items[index].group_id,
                                     sizeof(model->items[index].group_id),
                                     target_group_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                (void)umi_mutex_unlock(model->mutex);
                return status;
            }
            changed = 1;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (changed) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/* Provide the should close operation used by this module and its client applications. */
static int should_close(const UmiUiDocumentViewSnapshot *item,
                        UmiUiDocumentCloseResult *result)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->dirty) {
        result->dirty_count += 1U;
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->pinned) {
        result->pinned_count += 1U;
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!item->closable) {
        result->non_closable_count += 1U;
        return 0;
    }
    return 1;
}

/*
 * Provide the ui document view model close others operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_close_others(
    UmiUiDocumentViewModel *model,
    const char *item_id,
    UmiUiDocumentCloseResult *out_result)
{
    UmiUiDocumentCloseResult result = {0};
    size_t keep;
    size_t index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || item_id == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    keep = find_item(model, item_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (keep == SIZE_MAX) {
        (void)umi_mutex_unlock(model->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < model->count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].view_id, item_id) != 0 &&
            same_group(&model->items[index], &model->items[keep]) &&
            should_close(&model->items[index], &result)) {
            erase_at(model, index);
            result.closed_count += 1U;
            keep = find_item(model, item_id);
            continue;
        }
        ++index;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.closed_count > 0U) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    *out_result = result;
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the ui document view model close all operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_document_view_model_close_all(
    UmiUiDocumentViewModel *model,
    UmiUiDocumentCloseResult *out_result)
{
    UmiUiDocumentCloseResult result = {0};
    size_t index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(model->mutex);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < model->count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (should_close(&model->items[index], &result)) {
            erase_at(model, index);
            result.closed_count += 1U;
            continue;
        }
        ++index;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.closed_count > 0U) {
        model->revision = umi_ui_next_revision(model->revision);
    }
    *out_result = result;
    (void)umi_mutex_unlock(model->mutex);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ui document view model group without
 * changing their state.
 */
size_t umi_ui_document_view_model_group_count(
    const UmiUiDocumentViewModel *model,
    const char *group_id)
{
    UmiUiDocumentViewSnapshot requested = {0};
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || group_id == NULL) return 0U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (copy_identifier(requested.group_id, sizeof(requested.group_id),
                        group_id) != UMI_STATUS_OK) {
        return 0U;
    }
    (void)umi_mutex_lock(model->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_group(&model->items[index], &requested)) ++count;
    }
    (void)umi_mutex_unlock(model->mutex);
    return count;
}
