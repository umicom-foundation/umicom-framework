/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/education_workspace.h
 * PURPOSE: Present educational workspace controls through GTK.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Education native adapter
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * Reusable presentation belongs in Framework; application modules stay thin. */
#ifndef UMICOM_UI_GTK4_EDUCATION_WORKSPACE_H
#define UMICOM_UI_GTK4_EDUCATION_WORKSPACE_H
#include <gtk/gtk.h>
#include "umicom/education_workspace/workspace.h"
#include "umicom/education_workspace/project_workflow.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiEducationGtkPanel UmiEducationGtkPanel;
/* Creates presentation only; no storage, compiler or child process is opened.
 * Panel owns one widget reference. Widget returns a borrowed pointer. */
UmiEducationGtkPanel *UmiEducationGtkCreate(void);
GtkWidget *UmiEducationGtkWidget(UmiEducationGtkPanel *panel);
/* Caller detaches the widget before Destroy. Signals and buffer callbacks are
 * disconnected even when a test or another view retains an old child widget. */
void UmiEducationGtkDestroy(UmiEducationGtkPanel *panel);
/* Borrow a connection for embedded hosts/tests. It must outlive the panel or a
 * later successful Bind. Rebinding refuses unsaved note edits. */
UmiStatus UmiEducationGtkBind(UmiEducationGtkPanel *panel,UmiDataServer *server,
    const char *id,const char *displayName);
void UmiEducationGtkAttachCloseGuard(UmiEducationGtkPanel *panel,GtkWindow *window);
/* A thin, stateless launcher locates its current GtkWindow only when clicked. */
GtkWidget *UmiEducationGtkLauncher(void);
void UmiEducationGtkOpen(GtkWindow *parent);
/** A host receives a copied successful export on its GTK thread. The callback
 * borrows metadata only for the call and may close the learning panel. It must
 * report adoption errors itself; no build or trust is implied by this action. */
typedef void (*UmiEducationGtkProjectOpen)(const UmiEducationProjectWorkflow *project, void *context);
/** Bind one host opener. On success the panel owns context through release;
 * failure leaves ownership with the caller. It is invoked only by the explicit
 * Open last exported project action after a complete export. */
UmiStatus UmiEducationGtkSetProjectOpener(UmiEducationGtkPanel *panel,
    UmiEducationGtkProjectOpen open, void *context, GDestroyNotify release);
/** Open a practicum with a host-owned project adoption action. Success transfers
 * context to the panel. The learning window can outlive parent, so the host
 * context must use a weak owner and refuse adoption after that owner closes. */
UmiStatus UmiEducationGtkOpenWithProjectOpener(GtkWindow *parent,
    UmiEducationGtkProjectOpen open, void *context, GDestroyNotify release);
int UmiEducationGtkRun(const char *applicationId,const char *title,int argc,char **argv);
#ifdef __cplusplus
}
#endif
#endif
