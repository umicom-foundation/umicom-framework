/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/views.h
 *
 * PURPOSE:
 *   Publish the public views contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_VIEWS_H
#define UMICOM_DESKTOP_VIEWS_H

#include "umicom/desktop/shell_model.h"
#include "umicom/ui/view_model.h"

UmiStatus umi_desktop_overview_view_create(
    const char *view_id,
    UmiDesktopShellModel *model,
    UmiUiViewModel **out_view);
UmiStatus umi_desktop_layout_designer_view_create(
    const char *view_id,
    UmiDesktopShellModel *model,
    UmiUiViewModel **out_view);
UmiStatus umi_desktop_monitor_manager_view_create(
    const char *view_id,
    UmiDesktopShellModel *model,
    UmiUiViewModel **out_view);

#endif
