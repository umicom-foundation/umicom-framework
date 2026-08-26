/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/inspector_binding.h
 *
 * PURPOSE:
 *   Describe inspector subject and editing binding paths.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_INSPECTOR_BINDING_H
#define UMICOM_UI_REACTIVE_INSPECTOR_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveInspectorBinding {
    char inspector_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char subject_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char edit_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveInspectorBinding;
void umi_ui_reactive_inspector_binding_init(UmiUiReactiveInspectorBinding *item);
int umi_ui_reactive_inspector_binding_valid(const UmiUiReactiveInspectorBinding *item);
#ifdef __cplusplus
}
#endif
#endif
