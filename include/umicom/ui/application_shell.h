/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/application_shell.h
 *
 * PURPOSE:
 *   Define the product-neutral application shell containing one workbench, title,
 *   status text, window bounds and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPLICATION_SHELL_H
#define UMICOM_UI_APPLICATION_SHELL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the ui application shell snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiUiApplicationShellSnapshot { char application_id[UMI_UI_ID_CAPACITY]; char title[UMI_UI_TEXT_CAPACITY]; char status_text[UMI_UI_DESCRIPTION_CAPACITY]; UmiUiRect bounds; int maximised; int full_screen; int visible; uint64_t revision; } UmiUiApplicationShellSnapshot;
/**
 * Represent the ui application shell data shared with callers of this public contract.
 */
typedef struct UmiUiApplicationShell UmiUiApplicationShell;
/**
 * Initialise ui application shell from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_application_shell_create(const char *application_id, const char *title, UmiUiWorkbench *workbench, UmiUiApplicationShell **out_shell);
/**
 * Release or reset state held by ui application shell so the same storage can be reused
 * safely.
 */
void umi_ui_application_shell_destroy(UmiUiApplicationShell *shell);
/**
 * Provide the ui application shell set title operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_application_shell_set_title(UmiUiApplicationShell *shell, const char *title);
/**
 * Provide the ui application shell set status operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_application_shell_set_status(UmiUiApplicationShell *shell, const char *status_text);
/**
 * Provide the ui application shell set bounds operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_application_shell_set_bounds(UmiUiApplicationShell *shell, UmiUiRect bounds);
/**
 * Provide the ui application shell set visible operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_application_shell_set_visible(UmiUiApplicationShell *shell, int visible);
/**
 * Provide the ui application shell set maximised operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_application_shell_set_maximised(UmiUiApplicationShell *shell, int maximised);
/**
 * Provide the ui application shell snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_application_shell_snapshot(const UmiUiApplicationShell *shell, UmiUiApplicationShellSnapshot *out_snapshot);
/**
 * Provide the ui application shell workbench operation used by this module and its client
 * applications.
 */
UmiUiWorkbench *umi_ui_application_shell_workbench(UmiUiApplicationShell *shell);

#ifdef __cplusplus
}
#endif

#endif
