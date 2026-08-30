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

UmiStatus umi_ws_inspector_model_init(UmiWsInspectorModel *model, const char *subject_id) {
    if (model == NULL || !umi_ws_id_valid(subject_id)) return UMI_STATUS_INVALID_ARGUMENT;
    *model = (UmiWsInspectorModel){0};
    return umi_ws_copy_text(model->subject_id, sizeof(model->subject_id), subject_id);
}

UmiStatus umi_ws_inspector_model_set(UmiWsInspectorModel *model,
                                     const char *property_id,
                                     const char *label,
                                     const char *value,
                                     bool editable) {
    size_t index;
    UmiWsInspectorProperty *property = NULL;
    if (model == NULL || !umi_ws_id_valid(property_id) || label == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->count; ++index) if (strcmp(model->properties[index].property_id, property_id) == 0) property = &model->properties[index];
    if (property == NULL) {
        if (model->count >= UMI_WS_MAX_PROPERTIES) return UMI_STATUS_CAPACITY_EXCEEDED;
        property = &model->properties[model->count++];
        *property = (UmiWsInspectorProperty){0};
        if (umi_ws_copy_text(property->property_id, sizeof(property->property_id), property_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_ws_copy_text(property->label, sizeof(property->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(property->value, sizeof(property->value), value) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    property->editable = editable;
    return UMI_STATUS_OK;
}

const UmiWsInspectorProperty *umi_ws_inspector_model_find(const UmiWsInspectorModel *model, const char *property_id) {
    size_t index;
    if (model == NULL || !umi_ws_id_valid(property_id)) return NULL;
    for (index = 0U; index < model->count; ++index) if (strcmp(model->properties[index].property_id, property_id) == 0) return &model->properties[index];
    return NULL;
}
