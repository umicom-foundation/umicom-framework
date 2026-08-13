/* Framework-owned task-monitor workbench view model. */
#ifndef UMICOM_TERMINAL_UI_TASK_VIEW_H
#define UMICOM_TERMINAL_UI_TASK_VIEW_H
#include "umicom/terminal_ui/controller.h"
#include "umicom/ui/view_model.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_terminal_ui_task_view_create(const char *view_id,
                                           UmiTerminalController *controller,
                                           UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
