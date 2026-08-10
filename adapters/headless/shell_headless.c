/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/headless/shell_headless.c
 *
 * PURPOSE:
 *   Render application-shell state and delegate workbench rendering for the headless
 *   reference adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "headless_internal.h"

UmiStatus umi_headless_render_shell(UmiUiHeadlessAdapter *adapter)
{
    UmiUiApplicationShellSnapshot shell;
    UmiUiWorkbench *workbench;
    UmiStatus status;
    status = umi_ui_application_shell_snapshot(adapter->shell, &shell);
    if (status != UMI_STATUS_OK) return status;
    status = umi_headless_append(adapter,
        "application=%s\ntitle=%s\nstatus=%s\nbounds=%d,%d,%d,%d\nvisible=%d\nmaximised=%d\n",
        shell.application_id, shell.title, shell.status_text,
        shell.bounds.x, shell.bounds.y, shell.bounds.width, shell.bounds.height,
        shell.visible, shell.maximised);
    if (status != UMI_STATUS_OK) return status;
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    return umi_headless_render_workbench(adapter, workbench);
}
