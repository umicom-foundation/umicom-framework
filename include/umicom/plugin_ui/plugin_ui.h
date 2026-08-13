/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin_ui/plugin_ui.h
 *
 * PURPOSE:
 *   Project Extension SDK and manager state into toolkit-neutral workbench
 *   views that GTK4, Qt, Wt or headless frontends can render.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_UI_PLUGIN_UI_H
#define UMICOM_PLUGIN_UI_PLUGIN_UI_H

#include "umicom/plugin/manager.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLUGIN_UI_VISIBLE_ROWS 32U

UmiStatus umi_plugin_ui_installed_view_create(const char *view_id,
                                              UmiPluginManager *manager,
                                              UmiUiViewModel **out_view);
UmiStatus umi_plugin_ui_catalogue_view_create(const char *view_id,
                                              UmiPluginManager *manager,
                                              UmiUiViewModel **out_view);
UmiStatus umi_plugin_ui_permissions_view_create(const char *view_id,
                                                UmiPluginManager *manager,
                                                UmiUiViewModel **out_view);
UmiStatus umi_plugin_ui_audit_view_create(const char *view_id,
                                          UmiPluginManager *manager,
                                          UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
