/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/inspector_model.c
 *
 * PURPOSE:
 *   Implement reusable property/inspector rows for GUI designers, media objects, trading orders and system resources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/inspector_model.h"

/*
 * Initialise ws inspector model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ws_inspector_model_init(UmiWsInspectorModel *model, const char *subject_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_ws_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *model = (UmiWsInspectorModel){0};
    return umi_ws_copy_text(model->subject_id, sizeof(model->subject_id), subject_id);
}

/*
 * Copy ws inspector model into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_ws_inspector_model_set(UmiWsInspectorModel *model,
                                     const char *property_id,
                                     const char *label,
                                     const char *value,
                                     bool editable) {
    size_t index;
    UmiWsInspectorProperty *property = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_ws_id_valid(property_id) || label == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(model->properties[index].property_id, property_id) == 0) property = &model->properties[index];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (property == NULL) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (model->count >= UMI_WS_MAX_PROPERTIES) return UMI_STATUS_CAPACITY_EXCEEDED;
        property = &model->properties[model->count++];
        *property = (UmiWsInspectorProperty){0};
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_ws_copy_text(property->property_id, sizeof(property->property_id), property_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(property->label, sizeof(property->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(property->value, sizeof(property->value), value) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    property->editable = editable;
    return UMI_STATUS_OK;
}

/*
 * Find ws inspector model while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsInspectorProperty *umi_ws_inspector_model_find(const UmiWsInspectorModel *model, const char *property_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_ws_id_valid(property_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(model->properties[index].property_id, property_id) == 0) return &model->properties[index];
    return NULL;
}
