/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/command.c
 *
 * PURPOSE:
 *   Execute non-structural commands and delegate structural commands through
 *   explicit helpers rather than frontend code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/command.h"
#include "internal.h"


/*
 * Initialise workbench designer command from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_command_init(
    UmiWorkbenchDesignerCommand *command,
    UmiWorkbenchDesignerCommandKind kind,
    const char *command_id)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL) return;
    (void)memset(command, 0, sizeof(*command));
    command->structure_size = sizeof(*command);
    command->kind = kind;
    command->node_kind = UMI_WORKBENCH_LAYOUT_NODE_PANEL;
    command->drop_zone = UMI_WORKBENCH_DESIGNER_DROP_CENTRE;
    command->number_value = 0.5;
    command->index_value = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_id != NULL) {
        (void)umi_workbench_designer_copy_text(
            command->command_id, sizeof(command->command_id), command_id);
    }
}

/*
 * Check that workbench designer command satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_designer_command_validate(
    const UmiWorkbenchDesignerCommand *command)
{
    /* Use the shared build helper when it is available from the parent composition. */
    if (command == NULL || command->structure_size != sizeof(*command) ||
        command->kind <= UMI_WORKBENCH_DESIGNER_COMMAND_NONE ||
        command->kind > UMI_WORKBENCH_DESIGNER_COMMAND_EXPORT ||
        command->command_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Select the behaviour associated with the requested command or state value. */
    switch (command->kind) {
        case UMI_WORKBENCH_DESIGNER_COMMAND_ADD_PANEL:
            /* Use the shared build helper when it is available from the parent composition. */
            if (command->component_id[0] == '\0' ||
                command->owner_application_id[0] == '\0') {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_WORKBENCH_DESIGNER_COMMAND_REMOVE_NODE:
        case UMI_WORKBENCH_DESIGNER_COMMAND_FLOAT_NODE:
        case UMI_WORKBENCH_DESIGNER_COMMAND_DOCK_NODE:
        case UMI_WORKBENCH_DESIGNER_COMMAND_SET_ACTIVE_TAB:
            /* Use the shared build helper when it is available from the parent composition. */
            if (command->target_node_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
            break;
        default:
            break;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise command result from caller-provided values so later operations receive a
 * known state.
 */
static void command_result_init(
    UmiWorkbenchDesignerCommandResult *result,
    const UmiWorkbenchLayoutDocument *document)
{
    (void)memset(result, 0, sizeof(*result));
    result->status = UMI_STATUS_OK;
    result->previous_revision = document->version.revision;
    result->resulting_revision = document->version.revision;
}

/* Provide the command add panel operation used by this module and its client applications. */
static UmiStatus command_add_panel(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerCommand *command,
    UmiWorkbenchDesignerCommandResult *result)
{
    UmiWorkbenchLayoutNode node;
    size_t node_index;
    const char *parent_id;
    UmiStatus status;
    umi_workbench_layout_node_init(
        &node,
        command->target_node_id[0] != '\0'
            ? command->target_node_id : command->command_id,
        command->node_kind);
    status = umi_workbench_layout_node_set_title(
        &node,
        command->text_value[0] != '\0'
            ? command->text_value : command->component_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_node_set_component(
            &node, command->component_id, command->owner_application_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_add_node(
            document, &node, &node_index);
    }
    parent_id = command->parent_node_id[0] != '\0'
        ? command->parent_node_id
        : document->nodes[document->root_index].node_id;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_attach_child(
            document, parent_id, node.node_id,
            command->index_value == UMI_WORKBENCH_DESIGNER_INDEX_NONE
                ? UMI_WORKBENCH_LAYOUT_MAX_CHILDREN : command->index_value);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        result->document_changed = true;
        (void)umi_workbench_designer_copy_text(
            result->affected_node_id,
            sizeof(result->affected_node_id),
            node.node_id);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (node_index < document->node_count) {
        (void)umi_workbench_layout_document_remove_node(
            document, node.node_id, true);
    }
    return status;
}

/*
 * Provide the command set property operation used by this module and its client
 * applications.
 */
static UmiStatus command_set_property(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerCommand *command,
    UmiWorkbenchDesignerCommandResult *result)
{
    UmiWorkbenchLayoutNode *node =
        umi_workbench_layout_document_find_node_mutable(
            document, command->target_node_id);
    UmiStatus status = UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(command->component_id, "title") == 0) {
        status = umi_workbench_layout_node_set_title(node, command->text_value);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command->component_id, "component") == 0) {
        status = umi_workbench_layout_node_set_component(
            node, command->text_value, node->owner_application_id);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command->component_id, "owner") == 0) {
        status = umi_workbench_layout_node_set_component(
            node, node->component_id, command->text_value);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command->component_id, "context-group") == 0) {
        status = umi_workbench_layout_node_set_context_group(
            node, command->text_value);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command->component_id, "split-ratio") == 0) {
        status = umi_workbench_layout_node_set_split(
            node, node->orientation, command->number_value);
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command->component_id, "visible") == 0) {
        node->visibility = command->bool_value
            ? UMI_WORKBENCH_LAYOUT_VISIBILITY_VISIBLE
            : UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
        status = UMI_STATUS_OK;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(command->component_id, "bounds") == 0) {
        UmiWorkbenchLayoutRect layout_rect =
            umi_workbench_designer_to_layout_rect(command->rect_value);
        status = umi_workbench_layout_node_set_bounds(node, &layout_rect);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        node->revision += 1U;
        result->document_changed = true;
        (void)umi_workbench_designer_copy_text(
            result->affected_node_id,
            sizeof(result->affected_node_id),
            node->node_id);
    }
    return status;
}

/*
 * Perform workbench designer command through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_designer_command_execute(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchDesignerCommand *command,
    UmiWorkbenchDesignerCommandResult *out_result)
{
    UmiStatus status;
    UmiWorkbenchLayoutOperation operation;
    UmiWorkbenchLayoutOperationResult operation_result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || command == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_designer_command_validate(command);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    command_result_init(out_result, document);
    /* Use the shared build helper when it is available from the parent composition. */
    if (command->expected_revision != 0U &&
        command->expected_revision != document->version.revision) {
        out_result->status = UMI_STATUS_INVALID_STATE;
        (void)umi_workbench_designer_copy_text(
            out_result->message, sizeof(out_result->message),
            "The layout changed after the command was prepared.");
        return out_result->status;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (command->kind) {
        case UMI_WORKBENCH_DESIGNER_COMMAND_ADD_PANEL:
            status = command_add_panel(document, command, out_result);
            break;
        case UMI_WORKBENCH_DESIGNER_COMMAND_REMOVE_NODE:
            status = umi_workbench_layout_document_remove_node(
                document, command->target_node_id, command->bool_value);
            out_result->document_changed = status == UMI_STATUS_OK;
            break;
        case UMI_WORKBENCH_DESIGNER_COMMAND_MOVE_NODE:
            status = umi_workbench_layout_document_move_node(
                document, command->target_node_id,
                command->parent_node_id, command->index_value);
            out_result->document_changed = status == UMI_STATUS_OK;
            break;
        case UMI_WORKBENCH_DESIGNER_COMMAND_SET_PROPERTY:
            status = command_set_property(document, command, out_result);
            break;
        case UMI_WORKBENCH_DESIGNER_COMMAND_RENAME_LAYOUT:
            umi_workbench_layout_operation_init(
                &operation,
                UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT,
                command->command_id);
            (void)umi_workbench_designer_copy_text(
                operation.text_value, sizeof(operation.text_value),
                command->text_value);
            operation.expected_revision = command->expected_revision;
            status = umi_workbench_layout_operation_apply(
                document, &operation, &operation_result);
            out_result->document_changed = operation_result.changed;
            break;
        case UMI_WORKBENCH_DESIGNER_COMMAND_LOCK_LAYOUT:
        case UMI_WORKBENCH_DESIGNER_COMMAND_UNLOCK_LAYOUT:
            status = umi_workbench_layout_document_set_flag(
                document,
                UMI_WORKBENCH_LAYOUT_DOCUMENT_LOCKED,
                command->kind == UMI_WORKBENCH_DESIGNER_COMMAND_LOCK_LAYOUT);
            out_result->document_changed = status == UMI_STATUS_OK;
            break;
        default:
            status = UMI_STATUS_NOT_IMPLEMENTED;
            break;
    }
    out_result->status = status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && out_result->document_changed) {
        umi_workbench_layout_document_increment_revision(document);
        umi_workbench_layout_document_refresh_hash(document);
        out_result->resulting_revision = document->version.revision;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (out_result->affected_node_id[0] == '\0' &&
            command->target_node_id[0] != '\0') {
            (void)umi_workbench_designer_copy_text(
                out_result->affected_node_id,
                sizeof(out_result->affected_node_id),
                command->target_node_id);
        }
        (void)umi_workbench_designer_copy_text(
            out_result->message, sizeof(out_result->message),
            "Designer command applied to the semantic layout.");
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK && out_result->message[0] == '\0') {
        (void)umi_workbench_designer_copy_text(
            out_result->message, sizeof(out_result->message),
            "Designer command could not be applied.");
    }
    return status;
}
