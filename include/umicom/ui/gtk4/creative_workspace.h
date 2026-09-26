/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/creative_workspace.h
 * PURPOSE: Expose a shared creative panel and a layout-preserving product surface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_CREATIVE_WORKSPACE_H
#define UMICOM_UI_GTK4_CREATIVE_WORKSPACE_H
#include <gtk/gtk.h>
#include "umicom/creative_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiCreativeGtkPanel UmiCreativeGtkPanel;
    typedef struct UmiCreativeGtkSurface UmiCreativeGtkSurface;
    /* The optional server is borrowed until Destroy. NULL selects explicit-open
 * local SQLite storage. Creating the panel does not open a database or project.
 * All calls, widget signals and disposal belong to the GTK owner thread. */
    UmiStatus UmiCreativeGtkPanelCreate(UmiDataServer * server, const char * profile, UmiCreativeGtkPanel * * outPanel);
    GtkWidget * UmiCreativeGtkPanelWidget(UmiCreativeGtkPanel * panel);
    void UmiCreativeGtkPanelDestroy(UmiCreativeGtkPanel * panel);
    /* A surface takes an unparented, borrowed original layout widget. The original
 * stays available under Existing layouts and is detached before destruction.
 * On failure it remains unparented. Both widget getters return borrowed refs. */
    UmiStatus UmiCreativeGtkSurfaceCreate(GtkWidget * original, const char * profile, UmiCreativeGtkSurface * * outSurface);
    GtkWidget * UmiCreativeGtkSurfaceWidget(UmiCreativeGtkSurface * surface);
    void UmiCreativeGtkSurfaceDestroy(UmiCreativeGtkSurface * surface);
    int UmiCreativeGtkRun(const char * applicationId, const char * title, const char * profile, int argc, char * * argv);
#ifdef __cplusplus
}

#endif
#endif
