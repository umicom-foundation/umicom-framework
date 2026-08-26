/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/list_binding.h
 *
 * PURPOSE:
 *   Describe list data-path and selection-path binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_LIST_BINDING_H
#define UMICOM_UI_REACTIVE_LIST_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveListBinding {
    char surface_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char items_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char selection_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveListBinding;
void umi_ui_reactive_list_binding_init(UmiUiReactiveListBinding *item);
int umi_ui_reactive_list_binding_valid(const UmiUiReactiveListBinding *item);
#ifdef __cplusplus
}
#endif
#endif
