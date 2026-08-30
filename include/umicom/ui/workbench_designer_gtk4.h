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

typedef struct _GtkWidget GtkWidget;
typedef struct UmiWorkbenchDesignerGtk4 UmiWorkbenchDesignerGtk4;
typedef struct UmiWorkbenchLayoutBrowserGtk4 UmiWorkbenchLayoutBrowserGtk4;

typedef void (*UmiWorkbenchDesignerGtk4CommandHandler)(
    const char *command_id,
    const char *session_id,
    void *user_data);

typedef struct UmiWorkbenchDesignerGtk4Config {
    UmiWorkbenchDesignerController *controller;
    UmiWorkbenchDesignerKeymap *keymap;
    UmiWorkbenchDesignerGtk4CommandHandler command_handler;
    void *command_user_data;
    bool show_browser;
    bool show_minimap;
    bool show_collaboration;
} UmiWorkbenchDesignerGtk4Config;

UmiStatus umi_workbench_designer_gtk4_create(
    const UmiWorkbenchDesignerGtk4Config *config,
    UmiWorkbenchDesignerGtk4 **out_designer);
void umi_workbench_designer_gtk4_destroy(
    UmiWorkbenchDesignerGtk4 *designer);
GtkWidget *umi_workbench_designer_gtk4_widget(
    UmiWorkbenchDesignerGtk4 *designer);
UmiStatus umi_workbench_designer_gtk4_refresh(
    UmiWorkbenchDesignerGtk4 *designer,
    uint64_t timestamp_ms);
UmiStatus umi_workbench_designer_gtk4_show_browser(
    UmiWorkbenchDesignerGtk4 *designer,
    bool visible);
UmiStatus umi_workbench_designer_gtk4_focus_canvas(
    UmiWorkbenchDesignerGtk4 *designer);

UmiStatus umi_workbench_layout_browser_gtk4_create(
    UmiWorkbenchDesignerController *controller,
    UmiWorkbenchLayoutBrowserGtk4 **out_browser);
void umi_workbench_layout_browser_gtk4_destroy(
    UmiWorkbenchLayoutBrowserGtk4 *browser);
GtkWidget *umi_workbench_layout_browser_gtk4_widget(
    UmiWorkbenchLayoutBrowserGtk4 *browser);
UmiStatus umi_workbench_layout_browser_gtk4_refresh(
    UmiWorkbenchLayoutBrowserGtk4 *browser,
    const UmiWorkbenchDesignerBrowser *model);

#ifdef __cplusplus
}
#endif

#endif
