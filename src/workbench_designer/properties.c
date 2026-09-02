/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/properties.c
 *
 * PURPOSE:
 *   Build property metadata and preserve mutation through the existing semantic
 *   operation contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/properties.h"
#include "internal.h"
#include <stdio.h>


/*
 * Initialise workbench designer property model from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_property_model_init(
    UmiWorkbenchDesignerPropertyModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
}

/* Add property only after its inputs and available capacity have been checked. */
static UmiStatus property_add(
    UmiWorkbenchDesignerPropertyModel *model,
    const char *property_id,
    const char *label,
    const char *category,
    UmiWorkbenchDesignerPropertyKind kind,
    bool editable,
    uint32_t order,
    UmiWorkbenchDesignerProperty **out_property)
{
    UmiWorkbenchDesignerProperty *property;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_WORKBENCH_DESIGNER_MAX_PROPERTIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    property = &model->properties[model->count++];
    (void)memset(property, 0, sizeof(*property));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_designer_copy_text(property->property_id, sizeof(property->property_id), property_id) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(property->label, sizeof(property->label), label) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(property->category, sizeof(property->category), category) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    property->kind = kind;
    property->editable = editable;
    property->order = order;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_property != NULL) *out_property = property;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer property model build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_property_model_build(
    UmiWorkbenchDesignerPropertyModel *model,
    const UmiWorkbenchLayoutDocument *document,
    const char *node_id)
{
    const UmiWorkbenchLayoutNode *node;
    UmiWorkbenchDesignerProperty *property;
    char buffer[64];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || document == NULL || node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node = umi_workbench_layout_document_find_node(document, node_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL) return UMI_STATUS_NOT_FOUND;
    umi_workbench_designer_property_model_init(model);
    (void)umi_workbench_designer_copy_text(model->node_id, sizeof(model->node_id), node_id);
    status = property_add(model, "title", "Title", "identity", UMI_WORKBENCH_DESIGNER_PROPERTY_TEXT, true, 10U, &property);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_designer_copy_text(property->text_value, sizeof(property->text_value), node->title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = property_add(model, "component", "Component", "identity", UMI_WORKBENCH_DESIGNER_PROPERTY_IDENTIFIER, true, 20U, &property);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_designer_copy_text(property->text_value, sizeof(property->text_value), node->component_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = property_add(model, "owner", "Owner application", "identity", UMI_WORKBENCH_DESIGNER_PROPERTY_IDENTIFIER, true, 30U, &property);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_designer_copy_text(property->text_value, sizeof(property->text_value), node->owner_application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = property_add(model, "context-group", "Context group", "interaction", UMI_WORKBENCH_DESIGNER_PROPERTY_IDENTIFIER, true, 40U, &property);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_workbench_designer_copy_text(property->text_value, sizeof(property->text_value), node->context_group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = property_add(model, "split-ratio", "Split ratio", "layout", UMI_WORKBENCH_DESIGNER_PROPERTY_NUMBER, node->kind == UMI_WORKBENCH_LAYOUT_NODE_SPLIT, 50U, &property);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) property->number_value = node->split_ratio;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = property_add(model, "visible", "Visible", "layout", UMI_WORKBENCH_DESIGNER_PROPERTY_BOOLEAN, true, 60U, &property);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) property->boolean_value = node->visibility != UMI_WORKBENCH_LAYOUT_VISIBILITY_HIDDEN;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = property_add(model, "bounds", "Bounds", "geometry", UMI_WORKBENCH_DESIGNER_PROPERTY_RECT, true, 70U, &property);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) property->rect_value = umi_workbench_designer_from_layout_rect(node->bounds);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = property_add(model, "dock", "Dock region", "layout", UMI_WORKBENCH_DESIGNER_PROPERTY_CHOICE, true, 80U, &property);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)snprintf(buffer, sizeof(buffer), "%d", (int)node->dock_region);
        status = umi_workbench_designer_copy_text(property->text_value, sizeof(property->text_value), buffer);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        model->document_revision = document->version.revision;
        model->revision += 1U;
    }
    return status;
}

/*
 * Provide the property find mutable operation used by this module and its client
 * applications.
 */
static UmiWorkbenchDesignerProperty *property_find_mutable(
    UmiWorkbenchDesignerPropertyModel *model,
    const char *property_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || property_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->properties[index].property_id, property_id) == 0) {
            return &model->properties[index];
        }
    }
    return NULL;
}

/*
 * Find workbench designer property while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiWorkbenchDesignerProperty *umi_workbench_designer_property_find(
    const UmiWorkbenchDesignerPropertyModel *model,
    const char *property_id)
{
    return property_find_mutable((UmiWorkbenchDesignerPropertyModel *)model, property_id);
}

/*
 * Provide the workbench designer property set text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_property_set_text(
    UmiWorkbenchDesignerPropertyModel *model,
    const char *property_id,
    const char *value)
{
    UmiWorkbenchDesignerProperty *property = property_find_mutable(model, property_id);
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL || value == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!property->editable || (property->kind != UMI_WORKBENCH_DESIGNER_PROPERTY_TEXT &&
        property->kind != UMI_WORKBENCH_DESIGNER_PROPERTY_IDENTIFIER &&
        property->kind != UMI_WORKBENCH_DESIGNER_PROPERTY_CHOICE)) return UMI_STATUS_INVALID_STATE;
    status = umi_workbench_designer_copy_text(property->text_value, sizeof(property->text_value), value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

/*
 * Provide the workbench designer property set number operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_property_set_number(
    UmiWorkbenchDesignerPropertyModel *model,
    const char *property_id,
    double value)
{
    UmiWorkbenchDesignerProperty *property = property_find_mutable(model, property_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!property->editable || property->kind != UMI_WORKBENCH_DESIGNER_PROPERTY_NUMBER) return UMI_STATUS_INVALID_STATE;
    property->number_value = value;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer property set boolean operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_property_set_boolean(
    UmiWorkbenchDesignerPropertyModel *model,
    const char *property_id,
    bool value)
{
    UmiWorkbenchDesignerProperty *property = property_find_mutable(model, property_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!property->editable || property->kind != UMI_WORKBENCH_DESIGNER_PROPERTY_BOOLEAN) return UMI_STATUS_INVALID_STATE;
    property->boolean_value = value;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer property to operation operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_property_to_operation(
    const UmiWorkbenchDesignerPropertyModel *model,
    const char *property_id,
    const char *actor_id,
    const char *correlation_id,
    uint64_t expected_revision,
    UmiWorkbenchLayoutOperation *out_operation)
{
    const UmiWorkbenchDesignerProperty *property;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || property_id == NULL || out_operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    property = umi_workbench_designer_property_find(model, property_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL) return UMI_STATUS_NOT_FOUND;
    umi_workbench_layout_operation_init(
        out_operation,
        UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT,
        property_id);
    (void)umi_workbench_designer_copy_text(out_operation->actor_id, sizeof(out_operation->actor_id), actor_id != NULL ? actor_id : "designer");
    (void)umi_workbench_designer_copy_text(out_operation->correlation_id, sizeof(out_operation->correlation_id), correlation_id != NULL ? correlation_id : property_id);
    (void)umi_workbench_designer_copy_text(out_operation->node_id, sizeof(out_operation->node_id), model->node_id);
    out_operation->expected_revision = expected_revision;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(property_id, "title") == 0) {
        out_operation->kind = UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT;
        (void)umi_workbench_designer_copy_text(out_operation->secondary_node_id, sizeof(out_operation->secondary_node_id), "title");
        return umi_workbench_designer_copy_text(out_operation->text_value, sizeof(out_operation->text_value), property->text_value);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(property_id, "component") == 0 || strcmp(property_id, "owner") == 0) {
        out_operation->kind = UMI_WORKBENCH_LAYOUT_OPERATION_SET_COMPONENT;
        (void)umi_workbench_designer_copy_text(out_operation->secondary_node_id, sizeof(out_operation->secondary_node_id), property_id);
        return umi_workbench_designer_copy_text(out_operation->text_value, sizeof(out_operation->text_value), property->text_value);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(property_id, "context-group") == 0) {
        out_operation->kind = UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP;
        return umi_workbench_designer_copy_text(out_operation->text_value, sizeof(out_operation->text_value), property->text_value);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(property_id, "split-ratio") == 0) {
        out_operation->kind = UMI_WORKBENCH_LAYOUT_OPERATION_SET_SPLIT_RATIO;
        out_operation->number_value = property->number_value;
        return UMI_STATUS_OK;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(property_id, "visible") == 0) {
        out_operation->kind = UMI_WORKBENCH_LAYOUT_OPERATION_SET_VISIBILITY;
        out_operation->bool_value = property->boolean_value;
        return UMI_STATUS_OK;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(property_id, "bounds") == 0) {
        out_operation->kind = UMI_WORKBENCH_LAYOUT_OPERATION_SET_BOUNDS;
        out_operation->rect_value = umi_workbench_designer_to_layout_rect(property->rect_value);
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_IMPLEMENTED;
}
