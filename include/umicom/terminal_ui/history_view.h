/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal_ui/history_view.h
 *
 * PURPOSE:
 *   Publish the public history view contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_UI_HISTORY_VIEW_H
#define UMICOM_TERMINAL_UI_HISTORY_VIEW_H
#include "umicom/terminal_ui/controller.h"
#include "umicom/ui/view_model.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_terminal_ui_history_view_create(const char *view_id,
                                              UmiTerminalController *controller,
                                              UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
