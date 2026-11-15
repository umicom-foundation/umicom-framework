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
#include "umicom/ai_workspace/reviewed_stream.h"
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
/* Borrow a transport created at single-threaded startup. Construction/attachment
 * sends no request. Approved canonical local jobs use reviewed streaming; other
 * providers retain complete-response execution. Keep transport alive until panel
 * release succeeds. Calls belong to the transport/GTK owner thread. */
UmiStatus UmiAiWorkspaceGtkPanelEnableLocalStreaming(UmiAiWorkspaceGtkPanel *panel,
    UmiAiLocalTransport *transport);
/* Retire controls, request cancellation, join/settle and release. Shutdown can
 * block for cooperative cancellation and storage. On failure retain *panel and
 * ALL borrowed services; retry release on its owner thread. NULL *panel is OK.
 * Prefer this checked interface when enabling streaming; the legacy void
 * destructor cannot report a retained native owner. */
UmiStatus UmiAiWorkspaceGtkPanelRelease(UmiAiWorkspaceGtkPanel **in_out_panel);
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
/* Attach before opening a library; an existing session returns BUSY. The
 * startup-created transport is borrowed through every later library session. */
UmiStatus UmiAiWorkspaceGtkSurfaceEnableLocalStreaming(UmiAiWorkspaceGtkSurface *surface,
    UmiAiLocalTransport *transport);
/* Retire library controls and release the complete borrowed session chain.
 * A failed panel/native release retains the surface for owner-thread retry. */
UmiStatus UmiAiWorkspaceGtkSurfaceRelease(UmiAiWorkspaceGtkSurface **in_out_surface);
#ifdef __cplusplus
}
#endif
#endif
