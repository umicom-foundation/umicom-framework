/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/document.c
 *
 * PURPOSE:
 *   Implement canonical layout document ownership, node topology mutations, metadata, revisioning and deterministic content hashes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/document.h"

#include <string.h>

#include "internal.h"

static void initialise_identity(
    UmiWorkbenchLayoutIdentity *identity,
    const char *layout_id)
{
    (void)memset(identity, 0, sizeof(*identity));
    if (layout_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            identity->layout_id,
            sizeof(identity->layout_id),
            layout_id,
            true);
    }
}

static void initialise_version(UmiWorkbenchLayoutVersion *version)
{
    (void)memset(version, 0, sizeof(*version));
    version->schema_version = UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION;
    version->revision = 1U;
    version->generation = 1U;
    version->base_revision = 0U;
}

static void reset_unused_nodes(
    UmiWorkbenchLayoutDocument *document,
    size_t start)
{
    size_t index;
    for (index = start; index < UMI_WORKBENCH_LAYOUT_MAX_NODES; ++index) {
        (void)memset(&document->nodes[index], 0, sizeof(document->nodes[index]));
    }
}

static UmiStatus detach_from_current_parent(
    UmiWorkbenchLayoutDocument *document,
    size_t child_index)
{
    UmiWorkbenchLayoutNode *child;
    UmiWorkbenchLayoutNode *parent;
    UmiStatus status;

    if (document == NULL ||
        !umi_workbench_layout_index_valid(child_index, document->node_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    child = &document->nodes[child_index];
    if (child->parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_OK;
    }
    if (!umi_workbench_layout_index_valid(
            child->parent_index, document->node_count)) {
        return UMI_STATUS_INVALID_STATE;
    }

    parent = &document->nodes[child->parent_index];
    status = umi_workbench_layout_node_remove_child_index(
        parent, child_index);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    child->parent_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    child->revision += 1U;
    return UMI_STATUS_OK;
}

static bool index_is_in_list(
    const size_t *indices,
    size_t count,
    size_t value)
{
    size_t index;
    for (index = 0U; index < count; ++index) {
        if (indices[index] == value) {
            return true;
        }
    }
    return false;
}

static void collect_subtree_indices(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index,
    size_t *indices,
    size_t *count)
{
    const UmiWorkbenchLayoutNode *node;
    size_t child;

    if (document == NULL || indices == NULL || count == NULL ||
        !umi_workbench_layout_index_valid(node_index, document->node_count) ||
        *count >= UMI_WORKBENCH_LAYOUT_MAX_NODES ||
        index_is_in_list(indices, *count, node_index)) {
        return;
    }

    indices[*count] = node_index;
    *count += 1U;
    node = &document->nodes[node_index];

    for (child = 0U; child < node->child_count; ++child) {
        collect_subtree_indices(
            document, node->child_indices[child], indices, count);
    }
}

static void remap_indices_after_compaction(
    UmiWorkbenchLayoutDocument *document,
    const size_t *old_to_new,
    size_t old_count)
{
    size_t node_index;

    for (node_index = 0U; node_index < document->node_count; ++node_index) {
        UmiWorkbenchLayoutNode *node = &document->nodes[node_index];
        size_t child_index;

        if (node->parent_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            if (node->parent_index < old_count) {
                node->parent_index = old_to_new[node->parent_index];
            } else {
                node->parent_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
            }
        }

        for (child_index = 0U;
             child_index < node->child_count;
             ++child_index) {
            const size_t old_child = node->child_indices[child_index];
            if (old_child < old_count) {
                node->child_indices[child_index] = old_to_new[old_child];
            } else {
                node->child_indices[child_index] =
                    UMI_WORKBENCH_LAYOUT_INDEX_NONE;
            }
        }
    }

    if (document->root_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
        document->root_index < old_count) {
        document->root_index = old_to_new[document->root_index];
    } else {
        document->root_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
}

static UmiStatus compact_removed_nodes(
    UmiWorkbenchLayoutDocument *document,
    const bool *remove_flags)
{
    UmiWorkbenchLayoutNode compacted[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t old_to_new[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t old_count;
    size_t old_index;
    size_t new_count = 0U;

    if (document == NULL || remove_flags == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    old_count = document->node_count;
    for (old_index = 0U; old_index < old_count; ++old_index) {
        old_to_new[old_index] = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
        if (!remove_flags[old_index]) {
            compacted[new_count] = document->nodes[old_index];
            old_to_new[old_index] = new_count;
            new_count += 1U;
        }
    }

    for (old_index = 0U; old_index < new_count; ++old_index) {
        document->nodes[old_index] = compacted[old_index];
    }
    document->node_count = new_count;
    remap_indices_after_compaction(document, old_to_new, old_count);
    reset_unused_nodes(document, new_count);
    return UMI_STATUS_OK;
}

void umi_workbench_layout_document_init(
    UmiWorkbenchLayoutDocument *document,
    const char *layout_id,
    const char *name)
{
    if (document == NULL) {
        return;
    }

    (void)memset(document, 0, sizeof(*document));
    document->structure_size = sizeof(*document);
    initialise_identity(&document->identity, layout_id);
    initialise_version(&document->version);
    document->root_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    document->content_hash = UMI_WORKBENCH_LAYOUT_FNV_OFFSET;

    if (name != NULL) {
        (void)umi_workbench_layout_copy_text(
            document->name,
            sizeof(document->name),
            name,
            true);
    }
}

UmiStatus umi_workbench_layout_document_copy(
    UmiWorkbenchLayoutDocument *destination,
    const UmiWorkbenchLayoutDocument *source)
{
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (source->structure_size < sizeof(*source) ||
        source->node_count > UMI_WORKBENCH_LAYOUT_MAX_NODES ||
        source->tag_count > UMI_WORKBENCH_LAYOUT_MAX_TAGS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *destination = *source;
    destination->structure_size = sizeof(*destination);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_validate_structure(
    const UmiWorkbenchLayoutDocument *document)
{
    size_t index;

    if (document == NULL ||
        document->structure_size < sizeof(*document)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_layout_text_present(document->identity.layout_id) ||
        !umi_workbench_layout_text_present(document->name)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (document->version.schema_version == 0U ||
        document->node_count > UMI_WORKBENCH_LAYOUT_MAX_NODES ||
        document->tag_count > UMI_WORKBENCH_LAYOUT_MAX_TAGS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (document->node_count == 0U) {
        if (document->root_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_INVALID_STATE;
        }
        return UMI_STATUS_OK;
    }
    if (!umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        return UMI_STATUS_INVALID_STATE;
    }

    for (index = 0U; index < document->node_count; ++index) {
        UmiStatus status =
            umi_workbench_layout_node_validate(&document->nodes[index]);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_set_identity(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutIdentity *identity)
{
    if (document == NULL || identity == NULL ||
        !umi_workbench_layout_text_present(identity->layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    document->identity = *identity;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_set_audit(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutAudit *audit)
{
    if (document == NULL || audit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    document->audit = *audit;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_set_metadata(
    UmiWorkbenchLayoutDocument *document,
    const char *name,
    const char *category,
    const char *description)
{
    UmiStatus status;

    if (document == NULL || name == NULL ||
        category == NULL || description == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_copy_text(
        document->name, sizeof(document->name), name, false);
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            document->category,
            sizeof(document->category),
            category,
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            document->description,
            sizeof(document->description),
            description,
            true);
    }
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_document_increment_revision(document);
    }
    return status;
}

UmiStatus umi_workbench_layout_document_add_tag(
    UmiWorkbenchLayoutDocument *document,
    const char *tag)
{
    size_t index;
    UmiStatus status;

    if (document == NULL || !umi_workbench_layout_text_present(tag)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (document->tag_count >= UMI_WORKBENCH_LAYOUT_MAX_TAGS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < document->tag_count; ++index) {
        if (umi_workbench_layout_text_equal_case_insensitive(
                document->tags[index].value, tag)) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    status = umi_workbench_layout_copy_text(
        document->tags[document->tag_count].value,
        sizeof(document->tags[document->tag_count].value),
        tag,
        false);
    if (status == UMI_STATUS_OK) {
        document->tag_count += 1U;
        umi_workbench_layout_document_increment_revision(document);
    }
    return status;
}

UmiStatus umi_workbench_layout_document_remove_tag(
    UmiWorkbenchLayoutDocument *document,
    const char *tag)
{
    size_t index;

    if (document == NULL || tag == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < document->tag_count; ++index) {
        if (umi_workbench_layout_text_equal_case_insensitive(
                document->tags[index].value, tag)) {
            size_t move;
            for (move = index;
                 move + 1U < document->tag_count;
                 ++move) {
                document->tags[move] = document->tags[move + 1U];
            }
            document->tag_count -= 1U;
            (void)memset(
                &document->tags[document->tag_count],
                0,
                sizeof(document->tags[document->tag_count]));
            umi_workbench_layout_document_increment_revision(document);
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

bool umi_workbench_layout_document_has_tag(
    const UmiWorkbenchLayoutDocument *document,
    const char *tag)
{
    size_t index;

    if (document == NULL || tag == NULL) {
        return false;
    }
    for (index = 0U; index < document->tag_count; ++index) {
        if (umi_workbench_layout_text_equal_case_insensitive(
                document->tags[index].value, tag)) {
            return true;
        }
    }
    return false;
}

UmiStatus umi_workbench_layout_document_add_node(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNode *node,
    size_t *out_index)
{
    UmiStatus status;
    size_t index;

    if (document == NULL || node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_node_validate(node);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (document->node_count >= UMI_WORKBENCH_LAYOUT_MAX_NODES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_workbench_layout_node_id_exists(document, node->node_id)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    index = document->node_count;
    document->nodes[index] = *node;
    document->nodes[index].structure_size =
        sizeof(document->nodes[index]);
    document->nodes[index].parent_index =
        UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    document->node_count += 1U;

    if (document->root_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        document->root_index = index;
    }
    if (out_index != NULL) {
        *out_index = index;
    }

    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_remove_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    bool remove_descendants)
{
    size_t remove_indices[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    bool remove_flags[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t remove_count = 0U;
    size_t node_index;
    size_t index;
    UmiStatus status;

    if (document == NULL || node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node_index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    if (node_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }

    if (!remove_descendants &&
        document->nodes[node_index].child_count > 0U) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = detach_from_current_parent(document, node_index);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)memset(remove_flags, 0, sizeof(remove_flags));
    if (remove_descendants) {
        collect_subtree_indices(
            document, node_index, remove_indices, &remove_count);
    } else {
        remove_indices[0] = node_index;
        remove_count = 1U;
    }
    for (index = 0U; index < remove_count; ++index) {
        remove_flags[remove_indices[index]] = true;
    }

    if (remove_flags[document->root_index]) {
        document->root_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    status = compact_removed_nodes(document, remove_flags);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    if (document->node_count > 0U &&
        document->root_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        for (index = 0U; index < document->node_count; ++index) {
            if (document->nodes[index].parent_index ==
                UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
                document->root_index = index;
                break;
            }
        }
    }

    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_set_root(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t index;

    if (document == NULL || node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (document->nodes[index].parent_index !=
        UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_INVALID_STATE;
    }

    document->root_index = index;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_attach_child(
    UmiWorkbenchLayoutDocument *document,
    const char *parent_node_id,
    const char *child_node_id,
    size_t position)
{
    size_t parent_index;
    size_t child_index;
    UmiWorkbenchLayoutNode *parent;
    UmiWorkbenchLayoutNode *child;
    UmiStatus status;

    if (document == NULL || parent_node_id == NULL ||
        child_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    parent_index = umi_workbench_layout_document_find_node_index(
        document, parent_node_id);
    child_index = umi_workbench_layout_document_find_node_index(
        document, child_node_id);
    if (parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE ||
        child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (parent_index == child_index) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    parent = &document->nodes[parent_index];
    child = &document->nodes[child_index];
    if (!umi_workbench_layout_node_accepts_child_kind(
            parent, child->kind)) {
        return UMI_STATUS_INVALID_STATE;
    }

    {
        size_t ancestor = parent_index;
        while (ancestor != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            if (ancestor == child_index) {
                return UMI_STATUS_INVALID_STATE;
            }
            ancestor = document->nodes[ancestor].parent_index;
        }
    }

    status = detach_from_current_parent(document, child_index);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_workbench_layout_node_add_child_index(
        parent, child_index, position);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    child->parent_index = parent_index;
    child->revision += 1U;
    if (document->root_index == child_index) {
        document->root_index = parent_index;
    }
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_detach_child(
    UmiWorkbenchLayoutDocument *document,
    const char *parent_node_id,
    const char *child_node_id)
{
    size_t parent_index;
    size_t child_index;
    UmiStatus status;

    if (document == NULL || parent_node_id == NULL ||
        child_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    parent_index = umi_workbench_layout_document_find_node_index(
        document, parent_node_id);
    child_index = umi_workbench_layout_document_find_node_index(
        document, child_node_id);
    if (parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE ||
        child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (document->nodes[child_index].parent_index != parent_index) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_workbench_layout_node_remove_child_index(
        &document->nodes[parent_index], child_index);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    document->nodes[child_index].parent_index =
        UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    document->nodes[child_index].revision += 1U;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_document_move_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    const char *new_parent_node_id,
    size_t position)
{
    return umi_workbench_layout_document_attach_child(
        document, new_parent_node_id, node_id, position);
}

UmiWorkbenchLayoutNode *umi_workbench_layout_document_find_node_mutable(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return NULL;
    }
    return &document->nodes[index];
}

const UmiWorkbenchLayoutNode *umi_workbench_layout_document_find_node(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return NULL;
    }
    return &document->nodes[index];
}

size_t umi_workbench_layout_document_find_node_index(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t index;

    if (document == NULL || node_id == NULL) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    for (index = 0U; index < document->node_count; ++index) {
        if (umi_workbench_layout_text_equal(
                document->nodes[index].node_id, node_id)) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

const UmiWorkbenchLayoutNode *umi_workbench_layout_document_root(
    const UmiWorkbenchLayoutDocument *document)
{
    if (document == NULL ||
        !umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        return NULL;
    }
    return &document->nodes[document->root_index];
}

UmiWorkbenchLayoutNode *umi_workbench_layout_document_root_mutable(
    UmiWorkbenchLayoutDocument *document)
{
    if (document == NULL ||
        !umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        return NULL;
    }
    return &document->nodes[document->root_index];
}

bool umi_workbench_layout_document_has_flag(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutDocumentFlags flag)
{
    return document != NULL &&
           (document->flags & (uint32_t)flag) == (uint32_t)flag;
}

UmiStatus umi_workbench_layout_document_set_flag(
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutDocumentFlags flag,
    bool enabled)
{
    uint32_t previous_flags;

    if (document == NULL || flag == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    previous_flags = document->flags;
    if (enabled) {
        document->flags |= (uint32_t)flag;
    } else {
        document->flags &= ~(uint32_t)flag;
    }
    if (document->flags == previous_flags) {
        return UMI_STATUS_OK;
    }

    /*
     * Dirty is working-state evidence, not a semantic user edit. Changing it
     * must refresh the snapshot hash without making the document dirty again
     * or creating an artificial content revision.
     */
    if (flag == UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY) {
        umi_workbench_layout_document_refresh_hash(document);
        return UMI_STATUS_OK;
    }

    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_layout_document_calculate_hash(
    const UmiWorkbenchLayoutDocument *document)
{
    uint64_t hash = UMI_WORKBENCH_LAYOUT_FNV_OFFSET;
    size_t index;

    if (document == NULL) {
        return 0U;
    }

    hash = umi_workbench_layout_hash_text(
        hash, document->identity.layout_id);
    hash = umi_workbench_layout_hash_text(
        hash, document->identity.owner_user_id);
    hash = umi_workbench_layout_hash_text(
        hash, document->identity.owner_application_id);
    hash = umi_workbench_layout_hash_text(
        hash, document->identity.workspace_id);
    hash = umi_workbench_layout_hash_text(hash, document->name);
    hash = umi_workbench_layout_hash_text(hash, document->category);
    hash = umi_workbench_layout_hash_text(hash, document->description);
    hash = umi_workbench_layout_hash_u64(
        hash, document->version.schema_version);
    hash = umi_workbench_layout_hash_size(hash, document->root_index);
    hash = umi_workbench_layout_hash_u64(hash, document->flags);

    for (index = 0U; index < document->tag_count; ++index) {
        hash = umi_workbench_layout_hash_text(
            hash, document->tags[index].value);
    }

    for (index = 0U; index < document->node_count; ++index) {
        const UmiWorkbenchLayoutNode *node = &document->nodes[index];
        size_t child;

        hash = umi_workbench_layout_hash_text(hash, node->node_id);
        hash = umi_workbench_layout_hash_text(hash, node->title);
        hash = umi_workbench_layout_hash_text(hash, node->component_id);
        hash = umi_workbench_layout_hash_text(
            hash, node->owner_application_id);
        hash = umi_workbench_layout_hash_text(
            hash, node->context_group_id);
        hash = umi_workbench_layout_hash_text(hash, node->monitor_id);
        hash = umi_workbench_layout_hash_u64(hash, node->kind);
        hash = umi_workbench_layout_hash_u64(hash, node->orientation);
        hash = umi_workbench_layout_hash_u64(hash, node->dock_region);
        hash = umi_workbench_layout_hash_u64(hash, node->visibility);
        hash = umi_workbench_layout_hash_double(hash, node->split_ratio);
        hash = umi_workbench_layout_hash_i32(hash, node->bounds.x);
        hash = umi_workbench_layout_hash_i32(hash, node->bounds.y);
        hash = umi_workbench_layout_hash_i32(hash, node->bounds.width);
        hash = umi_workbench_layout_hash_i32(hash, node->bounds.height);
        hash = umi_workbench_layout_hash_size(
            hash, node->parent_index);
        hash = umi_workbench_layout_hash_size(
            hash, node->active_child_index);
        hash = umi_workbench_layout_hash_u64(hash, node->flags);
        hash = umi_workbench_layout_hash_i32(hash, node->order);
        hash = umi_workbench_layout_hash_i32(hash, node->z_order);
        for (child = 0U; child < node->child_count; ++child) {
            hash = umi_workbench_layout_hash_size(
                hash, node->child_indices[child]);
        }
    }

    return hash;
}

void umi_workbench_layout_document_refresh_hash(
    UmiWorkbenchLayoutDocument *document)
{
    if (document != NULL) {
        document->content_hash =
            umi_workbench_layout_document_calculate_hash(document);
    }
}

void umi_workbench_layout_document_increment_revision(
    UmiWorkbenchLayoutDocument *document)
{
    if (document == NULL) {
        return;
    }
    document->version.revision += 1U;
    document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;
    umi_workbench_layout_document_refresh_hash(document);
}
