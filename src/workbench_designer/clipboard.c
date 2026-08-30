/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/clipboard.c
 *
 * PURPOSE:
 *   Implement subtree copy and remapping while preserving source evidence and
 *   avoiding identifier collisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/clipboard.h"
#include "internal.h"
#include <stdio.h>


void umi_workbench_designer_clipboard_init(UmiWorkbenchDesignerClipboard *clipboard)
{
    if (clipboard == NULL) return;
    (void)memset(clipboard, 0, sizeof(*clipboard));
}

void umi_workbench_designer_clipboard_clear(UmiWorkbenchDesignerClipboard *clipboard)
{
    if (clipboard == NULL) return;
    clipboard->node_count = 0U;
    clipboard->root_count = 0U;
    clipboard->cut = false;
    clipboard->source_layout_id[0] = '\0';
    clipboard->source_revision = 0U;
    clipboard->revision += 1U;
}

bool umi_workbench_designer_clipboard_empty(
    const UmiWorkbenchDesignerClipboard *clipboard)
{
    return clipboard == NULL || clipboard->node_count == 0U;
}

static bool clipboard_source_index(
    const size_t *source_indices,
    size_t count,
    size_t source_index,
    size_t *out_clipboard_index)
{
    size_t index;
    for (index = 0U; index < count; ++index) {
        if (source_indices[index] == source_index) {
            if (out_clipboard_index != NULL) *out_clipboard_index = index;
            return true;
        }
    }
    return false;
}

static UmiStatus clipboard_collect(
    UmiWorkbenchDesignerClipboard *clipboard,
    const UmiWorkbenchLayoutDocument *document,
    size_t source_index,
    size_t *source_indices,
    bool *visited)
{
    const UmiWorkbenchLayoutNode *source;
    UmiWorkbenchLayoutNode copy;
    size_t child_position;
    size_t clipboard_index;
    if (!umi_workbench_designer_document_index_valid(document, source_index)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (visited[source_index]) return UMI_STATUS_OK;
    if (clipboard->node_count >= UMI_WORKBENCH_DESIGNER_MAX_CLIPBOARD_NODES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    visited[source_index] = true;
    source = &document->nodes[source_index];
    clipboard_index = clipboard->node_count;
    copy = *source;
    copy.parent_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    copy.child_count = 0U;
    copy.active_child_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    clipboard->nodes[clipboard_index] = copy;
    source_indices[clipboard_index] = source_index;
    clipboard->node_count += 1U;
    for (child_position = 0U; child_position < source->child_count; ++child_position) {
        UmiStatus status = clipboard_collect(
            clipboard, document, source->child_indices[child_position],
            source_indices, visited);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_clipboard_copy(
    UmiWorkbenchDesignerClipboard *clipboard,
    const UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerSelection *selection,
    bool cut)
{
    bool visited[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t source_indices[UMI_WORKBENCH_DESIGNER_MAX_CLIPBOARD_NODES];
    size_t selected_index;
    size_t node_index;
    UmiStatus status = UMI_STATUS_OK;
    if (clipboard == NULL || document == NULL || selection == NULL ||
        selection->count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_clipboard_clear(clipboard);
    (void)memset(visited, 0, sizeof(visited));
    for (selected_index = 0U; selected_index < selection->count; ++selected_index) {
        const char *node_id = selection->items[selected_index].value;
        node_index = umi_workbench_layout_document_find_node_index(document, node_id);
        if (node_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
        if (clipboard->root_count >= UMI_WORKBENCH_DESIGNER_MAX_SELECTIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = clipboard_collect(
            clipboard, document, node_index, source_indices, visited);
        if (status != UMI_STATUS_OK) return status;
        if (clipboard_source_index(source_indices, clipboard->node_count,
                                   node_index, &node_index)) {
            clipboard->root_indices[clipboard->root_count++] = node_index;
        }
    }
    for (node_index = 0U; node_index < clipboard->node_count; ++node_index) {
        const UmiWorkbenchLayoutNode *source =
            &document->nodes[source_indices[node_index]];
        UmiWorkbenchLayoutNode *copy = &clipboard->nodes[node_index];
        size_t child_position;
        for (child_position = 0U; child_position < source->child_count; ++child_position) {
            size_t mapped;
            if (clipboard_source_index(source_indices, clipboard->node_count,
                                       source->child_indices[child_position], &mapped)) {
                copy->child_indices[copy->child_count++] = mapped;
            }
        }
        if (source->active_child_index < source->child_count) {
            size_t mapped;
            if (clipboard_source_index(source_indices, clipboard->node_count,
                    source->child_indices[source->active_child_index], &mapped)) {
                copy->active_child_index = mapped;
            }
        }
    }
    clipboard->cut = cut;
    (void)umi_workbench_designer_copy_text(
        clipboard->source_layout_id, sizeof(clipboard->source_layout_id),
        document->identity.layout_id);
    clipboard->source_revision = document->version.revision;
    clipboard->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus clipboard_unique_id(
    const UmiWorkbenchLayoutDocument *document,
    const char *source_id,
    const char *suffix,
    size_t ordinal,
    char *out_id,
    size_t capacity)
{
    char candidate[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char semantic_suffix[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    const char *requested_suffix =
        suffix != NULL && suffix[0] != '\0' ? suffix : "copy";
    size_t attempt;
    int written;

    if (requested_suffix[0] == '-') {
        written = snprintf(
            semantic_suffix, sizeof(semantic_suffix), "%s", requested_suffix);
    } else {
        written = snprintf(
            semantic_suffix, sizeof(semantic_suffix), "-%s", requested_suffix);
    }
    if (written < 0 || (size_t)written >= sizeof(semantic_suffix)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (attempt = 0U; attempt < 1000U; ++attempt) {
        if (ordinal == 0U && attempt == 0U) {
            written = snprintf(
                candidate, sizeof(candidate), "%s%s",
                source_id, semantic_suffix);
        } else if (attempt == 0U) {
            written = snprintf(
                candidate, sizeof(candidate), "%s%s-%zu",
                source_id, semantic_suffix, ordinal + 1U);
        } else {
            written = snprintf(
                candidate, sizeof(candidate), "%s%s-%zu-%zu",
                source_id, semantic_suffix, ordinal + 1U, attempt);
        }
        if (written < 0 || (size_t)written >= sizeof(candidate)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (umi_workbench_layout_document_find_node(document, candidate) == NULL) {
            return umi_workbench_designer_copy_text(out_id, capacity, candidate);
        }
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus umi_workbench_designer_clipboard_paste(
    UmiWorkbenchDesignerClipboard *clipboard,
    UmiWorkbenchLayoutDocument *document,
    const char *parent_node_id,
    size_t position,
    const char *identifier_suffix,
    UmiWorkbenchDesignerSelection *out_selection)
{
    size_t destination_indices[UMI_WORKBENCH_DESIGNER_MAX_CLIPBOARD_NODES];
    char destination_ids[UMI_WORKBENCH_DESIGNER_MAX_CLIPBOARD_NODES]
        [UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    if (clipboard == NULL || document == NULL || parent_node_id == NULL ||
        clipboard->node_count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (out_selection != NULL) umi_workbench_designer_selection_clear(out_selection);
    for (index = 0U; index < clipboard->node_count; ++index) {
        status = clipboard_unique_id(
            document, clipboard->nodes[index].node_id,
            identifier_suffix, index,
            destination_ids[index], sizeof(destination_ids[index]));
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < clipboard->node_count; ++index) {
        UmiWorkbenchLayoutNode node = clipboard->nodes[index];
        size_t child_position;
        (void)umi_workbench_designer_copy_text(
            node.node_id, sizeof(node.node_id), destination_ids[index]);
        node.parent_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
        for (child_position = 0U; child_position < node.child_count; ++child_position) {
            node.child_indices[child_position] = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
        }
        node.child_count = 0U;
        node.active_child_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
        status = umi_workbench_layout_document_add_node(
            document, &node, &destination_indices[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < clipboard->node_count; ++index) {
        UmiWorkbenchLayoutNode *destination =
            &document->nodes[destination_indices[index]];
        const UmiWorkbenchLayoutNode *source = &clipboard->nodes[index];
        size_t child_position;
        for (child_position = 0U; child_position < source->child_count; ++child_position) {
            size_t source_child = source->child_indices[child_position];
            if (source_child >= clipboard->node_count) return UMI_STATUS_INVALID_STATE;
            destination->child_indices[destination->child_count++] =
                destination_indices[source_child];
            document->nodes[destination_indices[source_child]].parent_index =
                destination_indices[index];
        }
        if (source->active_child_index < clipboard->node_count) {
            size_t active_destination = destination_indices[source->active_child_index];
            destination->active_child_index =
                umi_workbench_layout_node_child_position(destination, active_destination);
        }
    }
    for (index = 0U; index < clipboard->root_count; ++index) {
        const size_t root = clipboard->root_indices[index];
        if (root >= clipboard->node_count) return UMI_STATUS_INVALID_STATE;
        status = umi_workbench_layout_document_attach_child(
            document, parent_node_id, destination_ids[root], position + index);
        if (status != UMI_STATUS_OK) return status;
        if (out_selection != NULL) {
            status = umi_workbench_designer_selection_add(
                out_selection, destination_ids[root], index == 0U);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    umi_workbench_layout_document_increment_revision(document);
    umi_workbench_layout_document_refresh_hash(document);
    if (clipboard->cut) umi_workbench_designer_clipboard_clear(clipboard);
    return UMI_STATUS_OK;
}
