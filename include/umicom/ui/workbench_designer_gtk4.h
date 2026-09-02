/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workbench_designer_gtk4.h
 *
 * PURPOSE:
 *   Expose GTK4 Layout Browser and visual designer widgets while keeping GTK
 *   object ownership out of toolkit-neutral Framework contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKBENCH_DESIGNER_GTK4_H
#define UMICOM_UI_WORKBENCH_DESIGNER_GTK4_H

#include "umicom/workbench_designer/workbench_designer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the gtk widget data shared with callers of this public contract.
 */
typedef struct _GtkWidget GtkWidget;
/**
 * Represent the workbench designer gtk4 data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchDesignerGtk4 UmiWorkbenchDesignerGtk4;
/**
 * Represent the workbench layout browser gtk4 data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutBrowserGtk4 UmiWorkbenchLayoutBrowserGtk4;

typedef void (*UmiWorkbenchDesignerGtk4CommandHandler)(
    const char *command_id,
    const char *session_id,
    void *user_data);

/**
 * Represent the workbench designer gtk4 config data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerGtk4Config {
    UmiWorkbenchDesignerController *controller;
    UmiWorkbenchDesignerKeymap *keymap;
    UmiWorkbenchDesignerGtk4CommandHandler command_handler;
    void *command_user_data;
    bool show_browser;
    bool show_minimap;
    bool show_collaboration;
} UmiWorkbenchDesignerGtk4Config;

/**
 * Initialise workbench designer gtk4 from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_designer_gtk4_create(
    const UmiWorkbenchDesignerGtk4Config *config,
    UmiWorkbenchDesignerGtk4 **out_designer);
/**
 * Release or reset state held by workbench designer gtk4 so the same storage can be reused
 * safely.
 */
void umi_workbench_designer_gtk4_destroy(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 widget operation used by this module and its client
 * applications.
 */
GtkWidget *umi_workbench_designer_gtk4_widget(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_gtk4_refresh(
    UmiWorkbenchDesignerGtk4 *designer,
    uint64_t timestamp_ms);
/**
 * Provide the workbench designer gtk4 show browser operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_gtk4_show_browser(
    UmiWorkbenchDesignerGtk4 *designer,
    bool visible);
/**
 * Provide the workbench designer gtk4 focus canvas operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_gtk4_focus_canvas(
    UmiWorkbenchDesignerGtk4 *designer);

/**
 * Initialise workbench layout browser gtk4 from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_workbench_layout_browser_gtk4_create(
    UmiWorkbenchDesignerController *controller,
    UmiWorkbenchLayoutBrowserGtk4 **out_browser);
/**
 * Release or reset state held by workbench layout browser gtk4 so the same storage can be
 * reused safely.
 */
void umi_workbench_layout_browser_gtk4_destroy(
    UmiWorkbenchLayoutBrowserGtk4 *browser);
/**
 * Provide the workbench layout browser gtk4 widget operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_layout_browser_gtk4_widget(
    UmiWorkbenchLayoutBrowserGtk4 *browser);
/**
 * Provide the workbench layout browser gtk4 refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_browser_gtk4_refresh(
    UmiWorkbenchLayoutBrowserGtk4 *browser,
    const UmiWorkbenchDesignerBrowser *model);

#ifdef __cplusplus
}
#endif

#endif
