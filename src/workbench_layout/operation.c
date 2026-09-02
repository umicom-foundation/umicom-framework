/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/operation.c
 *
 * PURPOSE:
 *   Implement typed layout mutations, revision checks, inverse construction and auditable operation results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_layout/operation.h"

#include <stdio.h>
#include <string.h>

#include "internal.h"

/* Provide the set result operation used by this module and its client applications. */
static UmiStatus set_result(
    UmiWorkbenchLayoutOperationResult *result,
    UmiStatus status,
    bool changed,
    uint64_t before_revision,
    uint64_t after_revision,
    const char *message)
{
    UmiStatus copy_status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) {
        return status;
    }
    (void)memset(result, 0, sizeof(*result));
    result->structure_size = sizeof(*result);
    result->status = status;
    result->changed = changed;
    result->previous_revision = before_revision;
    result->resulting_revision = after_revision;
    copy_status = umi_workbench_layout_copy_text(
        result->message,
        sizeof(result->message),
        message != NULL ? message : "",
        true);
    return copy_status == UMI_STATUS_OK ? status : copy_status;
}

/* Provide the apply add node operation used by this module and its client applications. */
static UmiStatus apply_add_node(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    size_t node_index;
    UmiStatus status;

    status = umi_workbench_layout_document_add_node(
        document, &operation->node_value, &node_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (umi_workbench_layout_text_present(operation->parent_node_id)) {
        status = umi_workbench_layout_document_attach_child(
            document,
            operation->parent_node_id,
            operation->node_value.node_id,
            operation->index_value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)umi_workbench_layout_document_remove_node(
                document, operation->node_value.node_id, true);
            return status;
        }
    }
    return UMI_STATUS_OK;
}

/* Provide the apply remove node operation used by this module and its client applications. */
static UmiStatus apply_remove_node(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    return umi_workbench_layout_document_remove_node(
        document, operation->node_id, operation->bool_value);
}

/* Provide the apply move node operation used by this module and its client applications. */
static UmiStatus apply_move_node(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    return umi_workbench_layout_document_move_node(
        document,
        operation->node_id,
        operation->parent_node_id,
        operation->index_value);
}

/* Provide the apply set root operation used by this module and its client applications. */
static UmiStatus apply_set_root(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    return umi_workbench_layout_document_set_root(
        document, operation->node_id);
}

/*
 * Provide the apply set split ratio operation used by this module and its client
 * applications.
 */
static UmiStatus apply_set_split_ratio(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiWorkbenchLayoutNode *node =
        umi_workbench_layout_document_find_node_mutable(
            document, operation->node_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_workbench_layout_node_set_split(
        node, node->orientation, operation->number_value);
}

/*
 * Provide the apply set active child operation used by this module and its client
 * applications.
 */
static UmiStatus apply_set_active_child(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiWorkbenchLayoutNode *node =
        umi_workbench_layout_document_find_node_mutable(
            document, operation->node_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->index_value != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
        operation->index_value >= node->child_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node->active_child_index = operation->index_value;
    node->revision += 1U;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

/* Provide the apply set bounds operation used by this module and its client applications. */
static UmiStatus apply_set_bounds(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiWorkbenchLayoutNode *node =
        umi_workbench_layout_document_find_node_mutable(
            document, operation->node_id);
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_workbench_layout_node_set_bounds(
        node, &operation->rect_value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_document_increment_revision(document);
    }
    return status;
}

/*
 * Provide the apply set visibility operation used by this module and its client
 * applications.
 */
static UmiStatus apply_set_visibility(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiWorkbenchLayoutNode *node =
        umi_workbench_layout_document_find_node_mutable(
            document, operation->node_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (operation->index_value < UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE ||
        operation->index_value > UMI_WORKBENCH_LAYOUT_VISIBILITY_AUTO) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node->visibility =
        (UmiWorkbenchLayoutVisibility)operation->index_value;
    node->revision += 1U;
    umi_workbench_layout_document_increment_revision(document);
    return UMI_STATUS_OK;
}

/*
 * Provide the apply set context group operation used by this module and its client
 * applications.
 */
static UmiStatus apply_set_context_group(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiWorkbenchLayoutNode *node =
        umi_workbench_layout_document_find_node_mutable(
            document, operation->node_id);
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_workbench_layout_node_set_context_group(
        node, operation->text_value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_document_increment_revision(document);
    }
    return status;
}

/*
 * Provide the apply set component operation used by this module and its client
 * applications.
 */
static UmiStatus apply_set_component(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiWorkbenchLayoutNode *node =
        umi_workbench_layout_document_find_node_mutable(
            document, operation->node_id);
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_workbench_layout_node_set_component(
        node,
        operation->text_value,
        operation->secondary_node_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_document_increment_revision(document);
    }
    return status;
}

/*
 * Provide the apply rename layout operation used by this module and its client
 * applications.
 */
static UmiStatus apply_rename_layout(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiStatus status = umi_workbench_layout_copy_text(
        document->name,
        sizeof(document->name),
        operation->text_value,
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        umi_workbench_layout_document_increment_revision(document);
    }
    return status;
}

/* Provide the apply set locked operation used by this module and its client applications. */
static UmiStatus apply_set_locked(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    return umi_workbench_layout_document_set_flag(
        document,
        UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED,
        operation->bool_value);
}

/* Provide the apply set dirty operation used by this module and its client applications. */
static UmiStatus apply_set_dirty(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->bool_value) {
        document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        document->flags &=
            ~(uint32_t)UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;
    }
    document->version.revision += 1U;
    umi_workbench_layout_document_refresh_hash(document);
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench layout operation from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_operation_init(
    UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationKind kind,
    const char *operation_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL) {
        return;
    }
    (void)memset(operation, 0, sizeof(*operation));
    operation->structure_size = sizeof(*operation);
    operation->kind = kind;
    operation->index_value = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    operation->node_value.structure_size =
        sizeof(operation->node_value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            operation->operation_id,
            sizeof(operation->operation_id),
            operation_id,
            true);
    }
}

/*
 * Check that workbench layout operation satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_layout_operation_validate(
    const UmiWorkbenchLayoutOperation *operation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL ||
        operation->structure_size < sizeof(*operation) ||
        operation->kind < UMI_WORKBENCH_LAYOUT_OPERATION_ADD_NODE ||
        operation->kind > UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_workbench_layout_text_present(operation->operation_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (operation->kind) {
    case UMI_WORKBENCH_LAYOUT_OPERATION_ADD_NODE:
        return umi_workbench_layout_node_validate(
            &operation->node_value);
    case UMI_WORKBENCH_LAYOUT_OPERATION_REMOVE_NODE:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_ROOT:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_SPLIT_RATIO:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_ACTIVE_CHILD:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_BOUNDS:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_VISIBILITY:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT:
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_workbench_layout_text_present(operation->node_id)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_MOVE_NODE:
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_workbench_layout_text_present(operation->node_id) ||
            !umi_workbench_layout_text_present(
                operation->parent_node_id)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT:
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_workbench_layout_text_present(
                operation->text_value)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_LOCKED:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY:
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Perform workbench layout operation through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_layout_operation_apply(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result)
{
    uint64_t before_revision;
    uint64_t before_hash;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || operation == NULL) {
        return set_result(
            out_result,
            UMI_STATUS_INVALID_ARGUMENT,
            false,
            0U,
            0U,
            "Document and operation are required.");
    }
    status = umi_workbench_layout_operation_validate(operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return set_result(
            out_result,
            status,
            false,
            document->version.revision,
            document->version.revision,
            "The operation is invalid.");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->expected_revision != 0U &&
        operation->expected_revision != document->version.revision) {
        return set_result(
            out_result,
            UMI_STATUS_INVALID_STATE,
            false,
            document->version.revision,
            document->version.revision,
            "The expected layout revision does not match.");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_workbench_layout_operation_requires_unlocked_document(
            operation) &&
        umi_workbench_layout_document_has_flag(
            document, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED)) {
        return set_result(
            out_result,
            UMI_STATUS_PERMISSION_DENIED,
            false,
            document->version.revision,
            document->version.revision,
            "The layout is locked.");
    }

    before_revision = document->version.revision;
    before_hash = document->content_hash;

    /* Select the behaviour associated with the requested command or state value. */
    switch (operation->kind) {
    case UMI_WORKBENCH_LAYOUT_OPERATION_ADD_NODE:
        status = apply_add_node(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_REMOVE_NODE:
        status = apply_remove_node(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_MOVE_NODE:
        status = apply_move_node(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_ROOT:
        status = apply_set_root(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_SPLIT_RATIO:
        status = apply_set_split_ratio(document, operation);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            umi_workbench_layout_document_increment_revision(document);
        }
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_ACTIVE_CHILD:
        status = apply_set_active_child(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_BOUNDS:
        status = apply_set_bounds(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_VISIBILITY:
        status = apply_set_visibility(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP:
        status = apply_set_context_group(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT:
        status = apply_set_component(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT:
        status = apply_rename_layout(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_LOCKED:
        status = apply_set_locked(document, operation);
        break;
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY:
        status = apply_set_dirty(document, operation);
        break;
    default:
        status = UMI_STATUS_INVALID_ARGUMENT;
        break;
    }

    return set_result(
        out_result,
        status,
        status == UMI_STATUS_OK &&
            (document->version.revision != before_revision ||
             document->content_hash != before_hash),
        before_revision,
        document->version.revision,
        status == UMI_STATUS_OK
            ? "The layout operation was applied."
            : "The layout operation failed.");
}

/*
 * Provide the workbench layout operation inverse operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_operation_inverse(
    const UmiWorkbenchLayoutDocument *before,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperation *out_inverse)
{
    const UmiWorkbenchLayoutNode *node;
    size_t node_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (before == NULL || operation == NULL || out_inverse == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_workbench_layout_operation_init(
        out_inverse,
        operation->kind,
        "inverse");
    (void)umi_workbench_layout_copy_text(
        out_inverse->actor_id,
        sizeof(out_inverse->actor_id),
        operation->actor_id,
        true);
    (void)umi_workbench_layout_copy_text(
        out_inverse->correlation_id,
        sizeof(out_inverse->correlation_id),
        operation->correlation_id,
        true);
    out_inverse->expected_revision = 0U;

    /* Select the behaviour associated with the requested command or state value. */
    switch (operation->kind) {
    case UMI_WORKBENCH_LAYOUT_OPERATION_ADD_NODE:
        out_inverse->kind =
            UMI_WORKBENCH_LAYOUT_OPERATION_REMOVE_NODE;
        (void)umi_workbench_layout_copy_text(
            out_inverse->node_id,
            sizeof(out_inverse->node_id),
            operation->node_value.node_id,
            false);
        out_inverse->bool_value = true;
        return UMI_STATUS_OK;

    case UMI_WORKBENCH_LAYOUT_OPERATION_REMOVE_NODE:
        node = umi_workbench_layout_document_find_node(
            before, operation->node_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (node == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
        out_inverse->kind = UMI_WORKBENCH_LAYOUT_OPERATION_ADD_NODE;
        out_inverse->node_value = *node;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->parent_index != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
            node->parent_index < before->node_count) {
            const UmiWorkbenchLayoutNode *parent =
                &before->nodes[node->parent_index];
            (void)umi_workbench_layout_copy_text(
                out_inverse->parent_node_id,
                sizeof(out_inverse->parent_node_id),
                parent->node_id,
                true);
            out_inverse->index_value =
                umi_workbench_layout_node_child_position(
                    parent,
                    umi_workbench_layout_document_find_node_index(
                        before, node->node_id));
        }
        return UMI_STATUS_OK;

    case UMI_WORKBENCH_LAYOUT_OPERATION_MOVE_NODE:
        node_index = umi_workbench_layout_document_find_node_index(
            before, operation->node_id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (node_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_NOT_FOUND;
        }
        node = &before->nodes[node_index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->parent_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_INVALID_STATE;
        }
        out_inverse->kind = UMI_WORKBENCH_LAYOUT_OPERATION_MOVE_NODE;
        (void)umi_workbench_layout_copy_text(
            out_inverse->node_id,
            sizeof(out_inverse->node_id),
            operation->node_id,
            false);
        (void)umi_workbench_layout_copy_text(
            out_inverse->parent_node_id,
            sizeof(out_inverse->parent_node_id),
            before->nodes[node->parent_index].node_id,
            false);
        out_inverse->index_value =
            umi_workbench_layout_node_child_position(
                &before->nodes[node->parent_index], node_index);
        return UMI_STATUS_OK;

    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_ROOT:
        out_inverse->kind = UMI_WORKBENCH_LAYOUT_OPERATION_SET_ROOT;
        /* Apply this branch only when its contract condition is satisfied. */
        if (before->root_index == UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
            return UMI_STATUS_INVALID_STATE;
        }
        (void)umi_workbench_layout_copy_text(
            out_inverse->node_id,
            sizeof(out_inverse->node_id),
            before->nodes[before->root_index].node_id,
            false);
        return UMI_STATUS_OK;

    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_SPLIT_RATIO:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_ACTIVE_CHILD:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_BOUNDS:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_VISIBILITY:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP:
    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT:
        node = umi_workbench_layout_document_find_node(
            before, operation->node_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (node == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
        (void)umi_workbench_layout_copy_text(
            out_inverse->node_id,
            sizeof(out_inverse->node_id),
            operation->node_id,
            false);
        /* Apply this branch only when its contract condition is satisfied. */
        if (operation->kind ==
            UMI_WORKBENCH_LAYOUT_OPERATION_SET_SPLIT_RATIO) {
            out_inverse->number_value = node->split_ratio;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (operation->kind ==
                   UMI_WORKBENCH_LAYOUT_OPERATION_SET_ACTIVE_CHILD) {
            out_inverse->index_value = node->active_child_index;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (operation->kind ==
                   UMI_WORKBENCH_LAYOUT_OPERATION_SET_BOUNDS) {
            out_inverse->rect_value = node->bounds;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (operation->kind ==
                   UMI_WORKBENCH_LAYOUT_OPERATION_SET_VISIBILITY) {
            out_inverse->index_value = node->visibility;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (operation->kind ==
                   UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP) {
            (void)umi_workbench_layout_copy_text(
                out_inverse->text_value,
                sizeof(out_inverse->text_value),
                node->context_group_id,
                true);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            (void)umi_workbench_layout_copy_text(
                out_inverse->text_value,
                sizeof(out_inverse->text_value),
                node->component_id,
                true);
            (void)umi_workbench_layout_copy_text(
                out_inverse->secondary_node_id,
                sizeof(out_inverse->secondary_node_id),
                node->owner_application_id,
                true);
        }
        return UMI_STATUS_OK;

    case UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT:
        out_inverse->kind =
            UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT;
        (void)umi_workbench_layout_copy_text(
            out_inverse->text_value,
            sizeof(out_inverse->text_value),
            before->name,
            false);
        return UMI_STATUS_OK;

    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_LOCKED:
        out_inverse->bool_value =
            umi_workbench_layout_document_has_flag(
                before, UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED);
        return UMI_STATUS_OK;

    case UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY:
        out_inverse->bool_value =
            umi_workbench_layout_document_has_flag(
                before, UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY);
        return UMI_STATUS_OK;

    default:
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
}

/*
 * Provide the workbench layout operation is metadata only operation used by this module
 * and its client applications.
 */
bool umi_workbench_layout_operation_is_metadata_only(
    const UmiWorkbenchLayoutOperation *operation)
{
    return operation != NULL &&
           (operation->kind ==
                UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT ||
            operation->kind ==
                UMI_WORKBENCH_LAYOUT_OPERATION_SET_LOCKED ||
            operation->kind ==
                UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY);
}

/*
 * Provide the workbench layout operation requires unlocked document operation used by this
 * module and its client applications.
 */
bool umi_workbench_layout_operation_requires_unlocked_document(
    const UmiWorkbenchLayoutOperation *operation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL) {
        return true;
    }
    return operation->kind !=
               UMI_WORKBENCH_LAYOUT_OPERATION_SET_LOCKED &&
           operation->kind !=
               UMI_WORKBENCH_LAYOUT_OPERATION_SET_DIRTY;
}
