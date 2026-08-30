/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/grid_binding.h
 *
 * PURPOSE:
 *   Describe enterprise grid provider/query/selection binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_GRID_BINDING_H
#define UMICOM_UI_REACTIVE_GRID_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveGridBinding {
    char grid_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char provider_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char query_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char selection_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveGridBinding;
void umi_ui_reactive_grid_binding_init(UmiUiReactiveGridBinding *item);
int umi_ui_reactive_grid_binding_valid(const UmiUiReactiveGridBinding *item);
#ifdef __cplusplus
}
#endif
#endif
