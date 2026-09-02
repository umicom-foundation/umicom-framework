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

/*
 * Provide the initialise identity operation used by this module and its client
 * applications.
 */
static void initialise_identity(
    UmiWorkbenchLayoutIdentity *identity,
    const char *layout_id)
{
    (void)memset(identity, 0, sizeof(*identity));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            identity->layout_id,
            sizeof(identity->layout_id),
            layout_id,
            true);
    }
}

/*
 * Provide the initialise version operation used by this module and its client
 * applications.
 */
static void initialise_version(UmiWorkbenchLayoutVersion *version)
{
    (void)memset(version, 0, sizeof(*version));
    version->schema_version = UMI_WORKBENCH_LAYOUT_SCHEMA_VERSION;
    version->revision = 1U;
    version->generation = 1U;
    version->base_revision = 0U;
}

/*
 * Provide the reset unused nodes operation used by this module and its client
 * applications.
 */
static void reset_unused_nodes(
    UmiWorkbenchLayoutDocument *document,
    size_t start)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = start; index < UMI_WORKBENCH_LAYOUT_MAX_NODES; ++index) {
        (void)memset(&document->nodes[index], 0, sizeof(document->nodes[index]));
    }
}

/*
 * Provide the detach from current parent operation used by this module and its client
 * applications.
 */
static UmiStatus detach_from_current_parent(
    UmiWorkbenchLayoutDocument *document,
    size_t child_index)
{
    UmiWorkbenchLayoutNode *child;
    UmiWorkbenchLayoutNode *parent;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL ||
        !umi_workbench_layout_index_valid(child_index, document->node_count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    child = &document->nodes[child_index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (child->parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_OK;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_layout_index_valid(
            child->parent_index, document->node_count)) {
        return UMI_STATUS_INVALID_STATE;
    }

    parent = &document->nodes[child->parent_index];
    status = umi_workbench_layout_node_remove_child_index(
        parent, child_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    child->parent_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    child->revision += 1U;
    return UMI_STATUS_OK;
}

/* Provide the index is in list operation used by this module and its client applications. */
static bool index_is_in_list(
    const size_t *indices,
    size_t count,
    size_t value)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (indices[index] == value) {
            return true;
        }
    }
    return false;
}

/*
 * Provide the collect subtree indices operation used by this module and its client
 * applications.
 */
static void collect_subtree_indices(
    const UmiWorkbenchLayoutDocument *document,
    size_t node_index,
    size_t *indices,
    size_t *count)
{
    const UmiWorkbenchLayoutNode *node;
    size_t child;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || indices == NULL || count == NULL ||
        !umi_workbench_layout_index_valid(node_index, document->node_count) ||
        *count >= UMI_WORKBENCH_LAYOUT_MAX_NODES ||
        index_is_in_list(indices, *count, node_index)) {
        return;
    }

    indices[*count] = node_index;
    *count += 1U;
    node = &document->nodes[node_index];

    /* Visit each bounded item once so every record receives the same rule. */
    for (child = 0U; child < node->child_count; ++child) {
        collect_subtree_indices(
            document, node->child_indices[child], indices, count);
    }
}

/*
 * Provide the remap indices after compaction operation used by this module and its client
 * applications.
 */
static void remap_indices_after_compaction(
    UmiWorkbenchLayoutDocument *document,
    const size_t *old_to_new,
    size_t old_count)
{
    size_t node_index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (node_index = 0U; node_index < document->node_count; ++node_index) {
        UmiWorkbenchLayoutNode *node = &document->nodes[node_index];
        size_t child_index;

        /* Apply this branch only when its contract condition is satisfied. */
        if (node->parent_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (node->parent_index < old_count) {
                node->parent_index = old_to_new[node->parent_index];
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                node->parent_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
            }
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (child_index = 0U;
             child_index < node->child_count;
             ++child_index) {
            const size_t old_child = node->child_indices[child_index];
            /* Apply this branch only when its contract condition is satisfied. */
            if (old_child < old_count) {
                node->child_indices[child_index] = old_to_new[old_child];
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                node->child_indices[child_index] =
                    UMI_WORKBENCH_LAYOUT_INDEX_NONE;
            }
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (document->root_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
        document->root_index < old_count) {
        document->root_index = old_to_new[document->root_index];
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        document->root_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
}

/*
 * Provide the compact removed nodes operation used by this module and its client
 * applications.
 */
static UmiStatus compact_removed_nodes(
    UmiWorkbenchLayoutDocument *document,
    const bool *remove_flags)
{
    UmiWorkbenchLayoutNode compacted[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t old_to_new[UMI_WORKBENCH_LAYOUT_MAX_NODES];
    size_t old_count;
    size_t old_index;
    size_t new_count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || remove_flags == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    old_count = document->node_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (old_index = 0U; old_index < old_count; ++old_index) {
        old_to_new[old_index] = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!remove_flags[old_index]) {
            compacted[new_count] = document->nodes[old_index];
            old_to_new[old_index] = new_count;
            new_count += 1U;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (old_index = 0U; old_index < new_count; ++old_index) {
        document->nodes[old_index] = compacted[old_index];
    }
    document->node_count = new_count;
    remap_indices_after_compaction(document, old_to_new, old_count);
    reset_unused_nodes(document, new_count);
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench layout document from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_document_init(
    UmiWorkbenchLayoutDocument *document,
    const char *layout_id,
    const char *name)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) {
        return;
    }

    (void)memset(document, 0, sizeof(*document));
    document->structure_size = sizeof(*document);
    initialise_identity(&document->identity, layout_id);
    initialise_version(&document->version);
    document->root_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    document->content_hash = UMI_WORKBENCH_LAYOUT_FNV_OFFSET;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name != NULL) {
        (void)umi_workbench_layout_copy_text(
            document->name,
            sizeof(document->name),
            name,
            true);
    }
}

/*
 * Copy workbench layout document into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_workbench_layout_document_copy(
    UmiWorkbenchLayoutDocument *destination,
    const UmiWorkbenchLayoutDocument *source)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (source->structure_size < sizeof(*source) ||
        source->node_count > UMI_WORKBENCH_LAYOUT_MAX_NODES ||
        source->tag_count > UMI_WORKBENCH_LAYOUT_MAX_TAGS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *destination = *source;
    destination->structure_size = sizeof(*destination);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout document validate structure operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_document_validate_structure(
    const UmiWorkbenchLayoutDocument *document)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL ||
        document->structure_size < sizeof(*document)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_workbench_layout_text_present(document->identity.layout_id) ||
        !umi_workbench_layout_text_present(document->name)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (document->version.schema_version == 0U ||
        document->node_count > UMI_WORKBENCH_LAYOUT_MAX_NODES ||
        document->tag_count > UMI_WORKBENCH_LAYOUT_MAX_TAGS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (document->node_count == 0U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (document->root_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_INVALID_STATE;
        }
        return UMI_STATUS_OK;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        UmiStatus status =
            umi_workbench_layout_node_validate(&document->nodes[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout document set identity operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_identity(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutIdentity *identity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || identity == NULL ||
        !umi_workbench_layout_text_present(identity->layout_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    document->identity = *identity;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout document set audit operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_audit(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutAudit *audit)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || audit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    document->audit = *audit;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout document set metadata operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_metadata(
    UmiWorkbenchLayoutDocument *document,
    const char *name,
    const char *category,
    const char *description)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || name == NULL ||
        category == NULL || description == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_workbench_layout_copy_text(
        document->name, sizeof(document->name), name, false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            document->category,
            sizeof(document->category),
            category,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            document->description,
            sizeof(document->description),
            description,
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_document_increment_revision(document);
    }
    return status;
}

/*
 * Provide the workbench layout document add tag operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_add_tag(
    UmiWorkbenchLayoutDocument *document,
    const char *tag)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || !umi_workbench_layout_text_present(tag)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (document->tag_count >= UMI_WORKBENCH_LAYOUT_MAX_TAGS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->tag_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        document->tag_count += 1U;
        umi_workbench_layout_document_increment_revision(document);
    }
    return status;
}

/*
 * Provide the workbench layout document remove tag operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_remove_tag(
    UmiWorkbenchLayoutDocument *document,
    const char *tag)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || tag == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->tag_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_workbench_layout_text_equal_case_insensitive(
                document->tags[index].value, tag)) {
            size_t move;
            /* Visit each bounded item once so every record receives the same rule. */
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

/*
 * Provide the workbench layout document has tag operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_document_has_tag(
    const UmiWorkbenchLayoutDocument *document,
    const char *tag)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || tag == NULL) {
        return false;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->tag_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_workbench_layout_text_equal_case_insensitive(
                document->tags[index].value, tag)) {
            return true;
        }
    }
    return false;
}

/*
 * Provide the workbench layout document add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_add_node(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutNode *node,
    size_t *out_index)
{
    UmiStatus status;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_node_validate(node);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (document->node_count >= UMI_WORKBENCH_LAYOUT_MAX_NODES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
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

    /* Apply this branch only when its contract condition is satisfied. */
    if (document->root_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        document->root_index = index;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_index != NULL) {
        *out_index = index;
    }

    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout document remove node operation used by this module and its
 * client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node_index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (node_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!remove_descendants &&
        document->nodes[node_index].child_count > 0U) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = detach_from_current_parent(document, node_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)memset(remove_flags, 0, sizeof(remove_flags));
    /* Apply this branch only when its contract condition is satisfied. */
    if (remove_descendants) {
        collect_subtree_indices(
            document, node_index, remove_indices, &remove_count);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        remove_indices[0] = node_index;
        remove_count = 1U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < remove_count; ++index) {
        remove_flags[remove_indices[index]] = true;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (remove_flags[document->root_index]) {
        document->root_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    status = compact_removed_nodes(document, remove_flags);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (document->node_count > 0U &&
        document->root_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < document->node_count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the workbench layout document set root operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_root(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (document->nodes[index].parent_index !=
        UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_INVALID_STATE;
    }

    document->root_index = index;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout document attach child operation used by this module and its
 * client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || parent_node_id == NULL ||
        child_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    parent_index = umi_workbench_layout_document_find_node_index(
        document, parent_node_id);
    child_index = umi_workbench_layout_document_find_node_index(
        document, child_node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE ||
        child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent_index == child_index) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    parent = &document->nodes[parent_index];
    child = &document->nodes[child_index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_layout_node_accepts_child_kind(
            parent, child->kind)) {
        return UMI_STATUS_INVALID_STATE;
    }

    {
        size_t ancestor = parent_index;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (ancestor != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (ancestor == child_index) {
                return UMI_STATUS_INVALID_STATE;
            }
            ancestor = document->nodes[ancestor].parent_index;
        }
    }

    status = detach_from_current_parent(document, child_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_workbench_layout_node_add_child_index(
        parent, child_index, position);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    child->parent_index = parent_index;
    child->revision += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (document->root_index == child_index) {
        document->root_index = parent_index;
    }
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout document detach child operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_detach_child(
    UmiWorkbenchLayoutDocument *document,
    const char *parent_node_id,
    const char *child_node_id)
{
    size_t parent_index;
    size_t child_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || parent_node_id == NULL ||
        child_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    parent_index = umi_workbench_layout_document_find_node_index(
        document, parent_node_id);
    child_index = umi_workbench_layout_document_find_node_index(
        document, child_node_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE ||
        child_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (document->nodes[child_index].parent_index != parent_index) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_workbench_layout_node_remove_child_index(
        &document->nodes[parent_index], child_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    document->nodes[child_index].parent_index =
        UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    document->nodes[child_index].revision += 1U;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout document move node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_move_node(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id,
    const char *new_parent_node_id,
    size_t position)
{
    return umi_workbench_layout_document_attach_child(
        document, new_parent_node_id, node_id, position);
}

/*
 * Provide the workbench layout document find node mutable operation used by this module
 * and its client applications.
 */
UmiWorkbenchLayoutNode *umi_workbench_layout_document_find_node_mutable(
    UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return NULL;
    }
    return &document->nodes[index];
}

/*
 * Provide the workbench layout document find node operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutNode *umi_workbench_layout_document_find_node(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t index = umi_workbench_layout_document_find_node_index(
        document, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        return NULL;
    }
    return &document->nodes[index];
}

/*
 * Provide the workbench layout document find node index operation used by this module and
 * its client applications.
 */
size_t umi_workbench_layout_document_find_node_index(
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || node_id == NULL) {
        return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->node_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_workbench_layout_text_equal(
                document->nodes[index].node_id, node_id)) {
            return index;
        }
    }
    return UMI_WORKBENCH_LAYOUT_INDEX_NONE;
}

/*
 * Provide the workbench layout document root operation used by this module and its client
 * applications.
 */
const UmiWorkbenchLayoutNode *umi_workbench_layout_document_root(
    const UmiWorkbenchLayoutDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL ||
        !umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        return NULL;
    }
    return &document->nodes[document->root_index];
}

/*
 * Provide the workbench layout document root mutable operation used by this module and its
 * client applications.
 */
UmiWorkbenchLayoutNode *umi_workbench_layout_document_root_mutable(
    UmiWorkbenchLayoutDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL ||
        !umi_workbench_layout_index_valid(
            document->root_index, document->node_count)) {
        return NULL;
    }
    return &document->nodes[document->root_index];
}

/*
 * Provide the workbench layout document has flag operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_document_has_flag(
    const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutDocumentFlags flag)
{
    return document != NULL &&
           (document->flags & (uint32_t)flag) == (uint32_t)flag;
}

/*
 * Provide the workbench layout document set flag operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_document_set_flag(
    UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchLayoutDocumentFlags flag,
    bool enabled)
{
    uint32_t previous_flags;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || flag == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    previous_flags = document->flags;
    /* Apply this operation only while the related capability or state is available. */
    if (enabled) {
        document->flags |= (uint32_t)flag;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        document->flags &= ~(uint32_t)flag;
    }
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the workbench layout document calculate hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_layout_document_calculate_hash(
    const UmiWorkbenchLayoutDocument *document)
{
    uint64_t hash = UMI_WORKBENCH_LAYOUT_FNV_OFFSET;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < document->tag_count; ++index) {
        hash = umi_workbench_layout_hash_text(
            hash, document->tags[index].value);
    }

    /* Visit each bounded item once so every record receives the same rule. */
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
        /* Visit each bounded item once so every record receives the same rule. */
        for (child = 0U; child < node->child_count; ++child) {
            hash = umi_workbench_layout_hash_size(
                hash, node->child_indices[child]);
        }
    }

    return hash;
}

/*
 * Provide the workbench layout document refresh hash operation used by this module and its
 * client applications.
 */
void umi_workbench_layout_document_refresh_hash(
    UmiWorkbenchLayoutDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document != NULL) {
        document->content_hash =
            umi_workbench_layout_document_calculate_hash(document);
    }
}

/*
 * Provide the workbench layout document increment revision operation used by this module
 * and its client applications.
 */
void umi_workbench_layout_document_increment_revision(
    UmiWorkbenchLayoutDocument *document)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) {
        return;
    }
    document->version.revision += 1U;
    document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;
    umi_workbench_layout_document_refresh_hash(document);
}
