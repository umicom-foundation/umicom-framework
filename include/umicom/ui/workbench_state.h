/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench_state.h
 *
 * PURPOSE:
 *   Capture serialisable workbench chrome and navigation state independently of a GUI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A product may persist this snapshot through its Session Store or Data Server. Framework itself does not choose the persistence backend.
 */

#ifndef UMICOM_UI_WORKBENCH_STATE_H
#define UMICOM_UI_WORKBENCH_STATE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/document_view.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_WORKBENCH_STATE_TEXT_CAPACITY 2048U
#define UMI_UI_EDITOR_SPLIT_RATIO_MIN 1000
#define UMI_UI_EDITOR_SPLIT_RATIO_MAX 9000
#define UMI_UI_EDITOR_SPLIT_RATIO_DEFAULT 5000

/**
 * List the named ui editor split mode values accepted by this public contract.
 */
typedef enum UmiUiEditorSplitMode {
    UMI_UI_EDITOR_SPLIT_SINGLE = 0,
    UMI_UI_EDITOR_SPLIT_COLUMNS = 1,
    UMI_UI_EDITOR_SPLIT_ROWS = 2
} UmiUiEditorSplitMode;

/**
 * Represent the ui workbench state data shared with callers of this public contract.
 */
typedef struct UmiUiWorkbenchState {
    char active_activity[UMI_UI_ID_CAPACITY];
    char active_view_container[UMI_UI_ID_CAPACITY];
    char active_perspective[UMI_UI_ID_CAPACITY];
    char active_document[UMI_UI_ID_CAPACITY];
    char active_workspace_profile[UMI_UI_ID_CAPACITY];
    int sidebar_visible;
    int auxiliary_sidebar_visible;
    int bottom_panel_visible;
    int32_t sidebar_size;
    int32_t auxiliary_sidebar_size;
    int32_t bottom_panel_size;
    char active_editor_group[UMI_UI_ID_CAPACITY];
    UmiUiEditorSplitMode editor_split_mode;
    int32_t editor_split_ratio;
    uint64_t revision;
} UmiUiWorkbenchState;

/**
 * Initialise ui workbench state from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_workbench_state_init(UmiUiWorkbenchState *state);
/**
 * Write ui workbench state in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_ui_workbench_state_encode(
    const UmiUiWorkbenchState *state,
    char *out_text,
    size_t capacity);
/**
 * Read ui workbench state into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_ui_workbench_state_decode(
    const char *text,
    UmiUiWorkbenchState *out_state);

#ifdef __cplusplus
}
#endif
#endif
