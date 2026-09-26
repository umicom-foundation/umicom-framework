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
#ifdef __cplusplus
}
#endif
#endif
