/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/state.h
 *
 * PURPOSE:
 *   Track common enabled, visible, busy and error state for reusable controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_STATE_H
#define UMICOM_UI_COMPONENTS_STATE_H
#include <stdbool.h>
typedef struct UmiUiComponentState { bool visible; bool enabled; bool busy; bool has_error; } UmiUiComponentState;
UmiUiComponentState umi_ui_component_state_default(void);
#endif
