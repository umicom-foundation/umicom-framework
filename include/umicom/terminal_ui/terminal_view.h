/* Framework-owned terminal workbench view model. */
#ifndef UMICOM_TERMINAL_UI_TERMINAL_VIEW_H
#define UMICOM_TERMINAL_UI_TERMINAL_VIEW_H
#include "umicom/terminal_ui/controller.h"
#include "umicom/ui/view_model.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_terminal_ui_terminal_view_create(const char *view_id,
                                               UmiTerminalController *controller,
                                               UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
