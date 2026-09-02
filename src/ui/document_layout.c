/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/document_layout.c
 *
 * PURPOSE:
 *   Capture, apply and serialise ordered editor-group layout metadata without
 *   coupling persistence or drag-and-drop behaviour to a GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/document_layout.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Provide the effective group operation used by this module and its client applications. */
static const char *effective_group(
    const UmiUiDocumentViewSnapshot *document)
{
    return document->group_id[0] != '\0'
        ? document->group_id
        : UMI_UI_PRIMARY_EDITOR_GROUP_ID;
}

/* Provide the position in group operation used by this module and its client applications. */
static int32_t position_in_group(const UmiUiDocumentViewModel *documents,
                                 size_t document_index,
                                 const char *group_id)
{
    size_t index;
    int32_t position = 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document_index; ++index) {
        UmiUiDocumentViewSnapshot document;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_document_view_model_at(documents, index, &document) ==
                UMI_STATUS_OK &&
            strcmp(effective_group(&document), group_id) == 0) {
            position += 1;
        }
    }
    return position;
}

/*
 * Initialise ui document layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_document_layout_init(UmiUiDocumentLayoutSnapshot *layout)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return;
    (void)memset(layout, 0, sizeof(*layout));
    layout->revision = 1U;
}

/*
 * Provide the ui document layout capture operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_document_layout_capture(
    const UmiUiDocumentViewModel *documents,
    UmiUiDocumentLayoutSnapshot *out_layout)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (documents == NULL || out_layout == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_ui_document_layout_init(out_layout);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_ui_document_view_model_count(documents);
         ++index) {
        UmiUiDocumentViewSnapshot document;
        UmiUiDocumentLayoutEntry *entry;
        const char *group_id;
        int written;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_layout->count >= UMI_UI_DOCUMENT_LAYOUT_ENTRY_MAX) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_ui_document_view_model_at(documents, index, &document) !=
            UMI_STATUS_OK) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
        entry = &out_layout->entries[out_layout->count];
        group_id = effective_group(&document);
        written = snprintf(entry->view_id, sizeof(entry->view_id), "%s",
                           document.view_id);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(entry->view_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        written = snprintf(entry->group_id, sizeof(entry->group_id), "%s",
                           group_id);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(entry->group_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        entry->position = position_in_group(documents, index, group_id);
        entry->active = document.active != 0;
        out_layout->count += 1U;
    }
    out_layout->revision = umi_ui_document_view_model_revision(documents);
    return UMI_STATUS_OK;
}

/*
 * Perform ui document layout through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ui_document_layout_apply(
    UmiUiDocumentViewModel *documents,
    const UmiUiDocumentLayoutSnapshot *layout,
    UmiUiDocumentLayoutApplyResult *out_result)
{
    UmiUiDocumentLayoutApplyResult result = {0};
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (documents == NULL || layout == NULL || out_result == NULL ||
        layout->count > UMI_UI_DOCUMENT_LAYOUT_ENTRY_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->count; ++index) {
        const UmiUiDocumentLayoutEntry *entry = &layout->entries[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_ui_id_is_valid(entry->view_id) ||
            !umi_ui_id_is_valid(entry->group_id) || entry->position < 0) {
            return UMI_STATUS_PARSE_ERROR;
        }
        status = umi_ui_document_view_model_place(
            documents, entry->view_id, entry->group_id,
            (size_t)entry->position);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) {
            result.missing_count += 1U;
            continue;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        result.applied_count += 1U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!layout->entries[index].active) continue;
        status = umi_ui_document_view_model_activate(
            documents, layout->entries[index].view_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND) {
            return status;
        }
    }
    *out_result = result;
    return UMI_STATUS_OK;
}

/*
 * Write ui document layout entry in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_ui_document_layout_entry_encode(
    const UmiUiDocumentLayoutEntry *entry,
    char *out_text,
    size_t capacity)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || out_text == NULL || capacity == 0U ||
        !umi_ui_id_is_valid(entry->view_id) ||
        !umi_ui_id_is_valid(entry->group_id) || entry->position < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_text, capacity, "v1|%s|%s|%" PRId32 "|%d",
                       entry->view_id, entry->group_id, entry->position,
                       entry->active != 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    return (size_t)written < capacity
        ? UMI_STATUS_OK
        : UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Read ui document layout entry into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ui_document_layout_entry_decode(
    const char *text,
    UmiUiDocumentLayoutEntry *out_entry)
{
    char version[8];
    char view_id[UMI_UI_ID_CAPACITY];
    char group_id[UMI_UI_ID_CAPACITY];
    char trailing;
    int position;
    int active;
    int matched;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    matched = sscanf(text, "%7[^|]|%63[^|]|%63[^|]|%d|%d%c",
                     version, view_id, group_id, &position, &active, &trailing);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (matched != 5 || strcmp(version, "v1") != 0 ||
        !umi_ui_id_is_valid(view_id) || !umi_ui_id_is_valid(group_id) ||
        position < 0 || (active != 0 && active != 1)) {
        return UMI_STATUS_PARSE_ERROR;
    }
    (void)memset(out_entry, 0, sizeof(*out_entry));
    (void)snprintf(out_entry->view_id, sizeof(out_entry->view_id), "%s",
                   view_id);
    (void)snprintf(out_entry->group_id, sizeof(out_entry->group_id), "%s",
                   group_id);
    out_entry->position = position;
    out_entry->active = active;
    return UMI_STATUS_OK;
}
