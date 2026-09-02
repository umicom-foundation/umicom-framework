/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/inspector_validation.c
 *
 * PURPOSE:
 *   Return precise property validation evidence for labels, identifiers, split
 *   ratios, geometry and read-only state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/inspector_validation.h"
#include "internal.h"
#include <ctype.h>
#include <math.h>


/*
 * Initialise workbench designer inspector validation from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_inspector_validation_init(
    UmiWorkbenchDesignerInspectorValidation *validation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (validation == NULL) return;
    (void)memset(validation, 0, sizeof(*validation));
    validation->status = UMI_STATUS_OK;
    validation->valid = true;
}

/* Provide the inspector fail operation used by this module and its client applications. */
static UmiStatus inspector_fail(
    UmiWorkbenchDesignerInspectorValidation *validation,
    const UmiWorkbenchDesignerProperty *property,
    UmiStatus status,
    UmiWorkbenchDesignerInspectorIssue issue,
    const char *message,
    const char *remediation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_inspector_validation_init(validation);
    validation->status = status;
    validation->issue = issue;
    validation->valid = false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property != NULL) {
        (void)umi_workbench_designer_copy_text(
            validation->property_id, sizeof(validation->property_id),
            property->property_id);
    }
    (void)umi_workbench_designer_copy_text(
        validation->message, sizeof(validation->message), message);
    (void)umi_workbench_designer_copy_text(
        validation->remediation, sizeof(validation->remediation), remediation);
    return status;
}

/*
 * Check that inspector identifier satisfies its contract before another service relies on
 * it.
 */
static bool inspector_identifier_valid(const char *value)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value[0] == '\0') return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; value[index] != '\0'; ++index) {
        const unsigned char character = (unsigned char)value[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!(isalnum(character) || character == '.' || character == '-' ||
              character == '_' || character == ':')) return false;
    }
    return true;
}

/*
 * Provide the workbench designer inspector validate text operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_inspector_validate_text(
    const UmiWorkbenchDesignerProperty *property,
    const UmiWorkbenchLayoutNode *node,
    const char *value,
    UmiWorkbenchDesignerInspectorValidation *out_validation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL || node == NULL || value == NULL || out_validation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_inspector_validation_init(out_validation);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!property->editable) {
        return inspector_fail(out_validation, property,
            UMI_STATUS_PERMISSION_DENIED,
            UMI_WORKBENCH_DESIGNER_INSPECTOR_READ_ONLY,
            "This property is read-only.",
            "Change the related structural command instead of editing the value directly.");
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if ((strcmp(property->property_id, "title") == 0 ||
         strcmp(property->property_id, "component") == 0) && value[0] == '\0') {
        return inspector_fail(out_validation, property,
            UMI_STATUS_INVALID_ARGUMENT,
            UMI_WORKBENCH_DESIGNER_INSPECTOR_REQUIRED,
            "The property requires a value.",
            "Enter a non-empty title or component identifier.");
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if ((property->kind == UMI_WORKBENCH_DESIGNER_PROPERTY_IDENTIFIER ||
         strstr(property->property_id, "_id") != NULL) &&
        value[0] != '\0' && !inspector_identifier_valid(value)) {
        return inspector_fail(out_validation, property,
            UMI_STATUS_INVALID_ARGUMENT,
            UMI_WORKBENCH_DESIGNER_INSPECTOR_INVALID_IDENTIFIER,
            "The identifier contains unsupported characters.",
            "Use letters, digits, dot, dash, underscore or colon.");
    }
    (void)node;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer inspector validate number operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_designer_inspector_validate_number(
    const UmiWorkbenchDesignerProperty *property,
    const UmiWorkbenchLayoutNode *node,
    double value,
    UmiWorkbenchDesignerInspectorValidation *out_validation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL || node == NULL || out_validation == NULL ||
        !isfinite(value)) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_inspector_validation_init(out_validation);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!property->editable) {
        return inspector_fail(out_validation, property,
            UMI_STATUS_PERMISSION_DENIED,
            UMI_WORKBENCH_DESIGNER_INSPECTOR_READ_ONLY,
            "This property is read-only.",
            "Use an enabled structural command.");
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(property->property_id, "split-ratio") == 0) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind != UMI_WORKBENCH_LAYOUT_NODE_SPLIT) {
            return inspector_fail(out_validation, property,
                UMI_STATUS_INVALID_STATE,
                UMI_WORKBENCH_DESIGNER_INSPECTOR_INCOMPATIBLE_NODE,
                "Only split nodes have a split ratio.",
                "Select a horizontal or vertical split container.");
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (value < 0.05 || value > 0.95) {
            return inspector_fail(out_validation, property,
                UMI_STATUS_INVALID_ARGUMENT,
                UMI_WORKBENCH_DESIGNER_INSPECTOR_OUT_OF_RANGE,
                "The split ratio must remain between 0.05 and 0.95.",
                "Choose a ratio that leaves both split children visible.");
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench designer inspector validate rect operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_inspector_validate_rect(
    const UmiWorkbenchDesignerProperty *property,
    const UmiWorkbenchLayoutNode *node,
    UmiWorkbenchDesignerRect value,
    UmiWorkbenchDesignerInspectorValidation *out_validation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL || node == NULL || out_validation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_inspector_validation_init(out_validation);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!property->editable) {
        return inspector_fail(out_validation, property,
            UMI_STATUS_PERMISSION_DENIED,
            UMI_WORKBENCH_DESIGNER_INSPECTOR_READ_ONLY,
            "This geometry is read-only.",
            "Unlock the layout or select a movable node.");
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_designer_rect_is_valid(&value)) {
        return inspector_fail(out_validation, property,
            UMI_STATUS_INVALID_ARGUMENT,
            UMI_WORKBENCH_DESIGNER_INSPECTOR_OUT_OF_RANGE,
            "The geometry must have positive width and height.",
            "Enter finite coordinates and positive dimensions.");
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (value.width < (double)node->minimum_size.width ||
        value.height < (double)node->minimum_size.height) {
        return inspector_fail(out_validation, property,
            UMI_STATUS_INVALID_ARGUMENT,
            UMI_WORKBENCH_DESIGNER_INSPECTOR_OUT_OF_RANGE,
            "The geometry is smaller than the component minimum size.",
            "Increase the width or height to satisfy the component contract.");
    }
    return UMI_STATUS_OK;
}
