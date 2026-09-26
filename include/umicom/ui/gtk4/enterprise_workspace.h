/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/enterprise_workspace.h
 * PURPOSE: Render shared enterprise work without putting data or policy logic in products.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_ENTERPRISE_WORKSPACE_H
#define UMICOM_UI_GTK4_ENTERPRISE_WORKSPACE_H
#include <gtk/gtk.h>
#include "umicom/enterprise_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiEnterpriseWorkspaceGtkPanel UmiEnterpriseWorkspaceGtkPanel;
/* This panel always uses labelled practice identities, including when a
 * workspace is supplied by a caller. It is not an authenticated UI. Production
 * hosts must supply trusted identity handling through their own core consumer.
 * GTK owner thread only. A supplied workspace is borrowed. NULL offers an
 * explicit Open practice storage action; construction itself does no I/O.
 * Destroy disconnects retained controls before releasing any owned services. */
UmiStatus UmiEnterpriseWorkspaceGtkPanelCreate(UmiEnterpriseWorkspace *workspace,
    const char *description, const char *applicationId, UmiEnterpriseWorkspaceGtkPanel **outPanel);
GtkWidget *UmiEnterpriseWorkspaceGtkPanelWidget(UmiEnterpriseWorkspaceGtkPanel *panel);
void UmiEnterpriseWorkspaceGtkPanelDestroy(UmiEnterpriseWorkspaceGtkPanel *panel);
int UmiEnterpriseWorkspaceGtkRun(const char *applicationId, const char *title, int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
