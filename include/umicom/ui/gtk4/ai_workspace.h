/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/ai_workspace.h
 *
 * PURPOSE:
 *   Compose the shared AI workspace with native product layouts and explicit callback lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_UI_GTK4_AI_WORKSPACE_H
#define UMICOM_UI_GTK4_AI_WORKSPACE_H
#include <gtk/gtk.h>
#include "umicom/ai_workspace/providers.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiWorkspaceGtkPanel UmiAiWorkspaceGtkPanel;
/* Services are borrowed. Destroy joins any generation worker before those
 * services can be released. Construction does not run a job or contact HTTP.
 * All panel functions run on the GTK thread. Retained controls are disconnected
 * on destruction, so they cannot invoke a released Framework workspace. */
UmiStatus UmiAiWorkspaceGtkPanelCreate(UmiAiWorkspace *workspace, UmiAiRuntime *runtime,
    UmiAiWorkspaceCancellation *cancellation, const char *storageDescription,
    const char *applicationId, UmiAiWorkspaceGtkPanel **outPanel);
GtkWidget *UmiAiWorkspaceGtkPanelWidget(UmiAiWorkspaceGtkPanel *panel);
void UmiAiWorkspaceGtkPanelDestroy(UmiAiWorkspaceGtkPanel *panel);
int UmiAiWorkspaceGtkRun(const char *applicationId, const char *title, int argc, char **argv);

/** A layout-preserving AI host that opens only user-selected libraries. */
typedef struct UmiAiWorkspaceGtkSurface UmiAiWorkspaceGtkSurface;
/** Report generation, file reading or chooser work that prevents library replacement. */
bool UmiAiWorkspaceGtkPanelBusy(const UmiAiWorkspaceGtkPanel *panel);
/** Create a library-selection surface; the original unparented widget is borrowed. */
UmiStatus UmiAiWorkspaceGtkSurfaceCreate(GtkWidget *original, const char *applicationId,
    UmiAiWorkspaceGtkSurface **outSurface);
/** Return the borrowed surface widget. */
GtkWidget *UmiAiWorkspaceGtkSurfaceWidget(UmiAiWorkspaceGtkSurface *surface);
/**
 * Open a chosen library after the caller has confirmed discarding unsaved forms.
 * A failure leaves the current session unchanged; active work returns BUSY.
 */
UmiStatus UmiAiWorkspaceGtkSurfaceOpenLibrary(UmiAiWorkspaceGtkSurface *surface,
    const char *path, bool create);
/** Retire controls and services, then detach the borrowed original layout. */
void UmiAiWorkspaceGtkSurfaceDestroy(UmiAiWorkspaceGtkSurface *surface);
#ifdef __cplusplus
}
#endif
#endif
