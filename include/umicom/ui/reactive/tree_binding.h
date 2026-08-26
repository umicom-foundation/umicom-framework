/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/tree_binding.h
 *
 * PURPOSE:
 *   Describe tree provider/expansion/selection binding paths.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_TREE_BINDING_H
#define UMICOM_UI_REACTIVE_TREE_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveTreeBinding {
    char tree_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char provider_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char expansion_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char selection_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveTreeBinding;
void umi_ui_reactive_tree_binding_init(UmiUiReactiveTreeBinding *item);
int umi_ui_reactive_tree_binding_valid(const UmiUiReactiveTreeBinding *item);
#ifdef __cplusplus
}
#endif
#endif
