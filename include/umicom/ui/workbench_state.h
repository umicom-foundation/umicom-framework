/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench_state.h
 *
 * PURPOSE:
 *   Capture serialisable workbench chrome and navigation state independently of a GUI toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A product may persist this snapshot through its Session Store or Data Server. Framework itself does not choose the persistence backend.
 */

#ifndef UMICOM_UI_WORKBENCH_STATE_H
#define UMICOM_UI_WORKBENCH_STATE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_WORKBENCH_STATE_TEXT_CAPACITY 2048U

typedef struct UmiUiWorkbenchState {
    char active_activity[UMI_UI_ID_CAPACITY];
    char active_view_container[UMI_UI_ID_CAPACITY];
    char active_perspective[UMI_UI_ID_CAPACITY];
    char active_document[UMI_UI_ID_CAPACITY];
    int sidebar_visible;
    int auxiliary_sidebar_visible;
    int bottom_panel_visible;
    int32_t sidebar_size;
    int32_t auxiliary_sidebar_size;
    int32_t bottom_panel_size;
    uint64_t revision;
} UmiUiWorkbenchState;

void umi_ui_workbench_state_init(UmiUiWorkbenchState *state);
UmiStatus umi_ui_workbench_state_encode(
    const UmiUiWorkbenchState *state,
    char *out_text,
    size_t capacity);
UmiStatus umi_ui_workbench_state_decode(
    const char *text,
    UmiUiWorkbenchState *out_state);

#ifdef __cplusplus
}
#endif
#endif
