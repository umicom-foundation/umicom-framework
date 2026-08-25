/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/inspector_model.h
 *
 * PURPOSE:
 *   Define reusable property/inspector rows for GUI designers, media objects, trading orders and system resources.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_INSPECTOR_MODEL_H
#define UMICOM_UI_WORKSTATION_INSPECTOR_MODEL_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsInspectorProperty {
    char property_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char value[UMI_UI_DESCRIPTION_CAPACITY];
    bool editable;
} UmiWsInspectorProperty;

typedef struct UmiWsInspectorModel {
    char subject_id[UMI_UI_ID_CAPACITY];
    UmiWsInspectorProperty properties[UMI_WS_MAX_PROPERTIES];
    size_t count;
} UmiWsInspectorModel;

UmiStatus umi_ws_inspector_model_init(UmiWsInspectorModel *model, const char *subject_id);
UmiStatus umi_ws_inspector_model_set(UmiWsInspectorModel *model,
                                     const char *property_id,
                                     const char *label,
                                     const char *value,
                                     bool editable);
const UmiWsInspectorProperty *umi_ws_inspector_model_find(const UmiWsInspectorModel *model, const char *property_id);

#ifdef __cplusplus
}
#endif

#endif
