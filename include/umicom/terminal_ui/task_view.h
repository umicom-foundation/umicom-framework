/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal_ui/task_view.h
 *
 * PURPOSE:
 *   Publish the public task view contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_UI_TASK_VIEW_H
#define UMICOM_TERMINAL_UI_TASK_VIEW_H
#include "umicom/terminal_ui/controller.h"
#include "umicom/ui/view_model.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise terminal ui task view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_terminal_ui_task_view_create(const char *view_id,
                                           UmiTerminalController *controller,
                                           UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
