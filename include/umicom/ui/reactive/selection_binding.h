/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/selection_binding.h
 *
 * PURPOSE:
 *   Synchronise canonical selection context with a semantic surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_SELECTION_BINDING_H
#define UMICOM_UI_REACTIVE_SELECTION_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveSelectionBinding {
    char surface_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char selection_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    bool two_way;
} UmiUiReactiveSelectionBinding;
void umi_ui_reactive_selection_binding_init(UmiUiReactiveSelectionBinding *item);
int umi_ui_reactive_selection_binding_valid(const UmiUiReactiveSelectionBinding *item);
#ifdef __cplusplus
}
#endif
#endif
