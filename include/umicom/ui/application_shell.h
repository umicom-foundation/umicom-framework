/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/application_shell.h
 *
 * PURPOSE:
 *   Define the product-neutral application shell containing one workbench, title,
 *   status text, window bounds and lifecycle state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPLICATION_SHELL_H
#define UMICOM_UI_APPLICATION_SHELL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiUiApplicationShellSnapshot { char application_id[UMI_UI_ID_CAPACITY]; char title[UMI_UI_TEXT_CAPACITY]; char status_text[UMI_UI_DESCRIPTION_CAPACITY]; UmiUiRect bounds; int maximised; int full_screen; int visible; uint64_t revision; } UmiUiApplicationShellSnapshot;
typedef struct UmiUiApplicationShell UmiUiApplicationShell;
UmiStatus umi_ui_application_shell_create(const char *application_id, const char *title, UmiUiWorkbench *workbench, UmiUiApplicationShell **out_shell);
void umi_ui_application_shell_destroy(UmiUiApplicationShell *shell);
UmiStatus umi_ui_application_shell_set_title(UmiUiApplicationShell *shell, const char *title);
UmiStatus umi_ui_application_shell_set_status(UmiUiApplicationShell *shell, const char *status_text);
UmiStatus umi_ui_application_shell_set_bounds(UmiUiApplicationShell *shell, UmiUiRect bounds);
UmiStatus umi_ui_application_shell_set_visible(UmiUiApplicationShell *shell, int visible);
UmiStatus umi_ui_application_shell_set_maximised(UmiUiApplicationShell *shell, int maximised);
UmiStatus umi_ui_application_shell_snapshot(const UmiUiApplicationShell *shell, UmiUiApplicationShellSnapshot *out_snapshot);
UmiUiWorkbench *umi_ui_application_shell_workbench(UmiUiApplicationShell *shell);

#ifdef __cplusplus
}
#endif

#endif
