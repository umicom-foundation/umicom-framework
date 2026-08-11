/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/state.h
 *
 * PURPOSE:
 *   Track common enabled, visible, busy and error state for reusable controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
