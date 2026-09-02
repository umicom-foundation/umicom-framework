/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal_ui/view_internal.h
 *
 * PURPOSE:
 *   Declare the view internal contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Private helpers for terminal/process/task view-model projections. */
#ifndef UMICOM_TERMINAL_UI_VIEW_INTERNAL_H
#define UMICOM_TERMINAL_UI_VIEW_INTERNAL_H

#include "umicom/ui/command_view.h"

/**
 * Provide the terminal ui base view operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_ui_base_view(const char *view_id,
                                    const char *kind,
                                    const char *title,
                                    const char *summary,
                                    UmiUiViewModel **out_view);
/**
 * Provide the terminal ui set string operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_ui_set_string(UmiUiViewModel *view,
                                     const char *key,
                                     const char *value);
/**
 * Provide the terminal ui set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_ui_set_integer(UmiUiViewModel *view,
                                      const char *key,
                                      int64_t value);
/**
 * Provide the terminal ui set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_ui_set_boolean(UmiUiViewModel *view,
                                      const char *key,
                                      int value);
/**
 * Provide the terminal ui set action operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_ui_set_action(UmiUiViewModel *view,
                                     size_t index,
                                     const char *action_id,
                                     const char *label,
                                     const char *tooltip);
#endif
