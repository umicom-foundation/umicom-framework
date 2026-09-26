/* Umicom Framework | Education native adapter
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * Reusable presentation belongs in Framework; application modules stay thin. */
#ifndef UMICOM_UI_GTK4_EDUCATION_WORKSPACE_H
#define UMICOM_UI_GTK4_EDUCATION_WORKSPACE_H
#include <gtk/gtk.h>
#include "umicom/education_workspace/workspace.h"
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
int UmiEducationGtkRun(const char *applicationId,const char *title,int argc,char **argv);
#ifdef __cplusplus
}
#endif
#endif
