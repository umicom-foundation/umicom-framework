/* Umicom Framework | Education native adapter
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * Reusable presentation belongs in Framework; application modules stay thin. */
#ifndef UMICOM_EDUCATION_WORKSPACE_GTK_PRIVATE_H
#define UMICOM_EDUCATION_WORKSPACE_GTK_PRIVATE_H
#include "umicom/ui/gtk4/education_workspace.h"
#include "umicom/education_workspace/projects.h"
struct UmiEducationGtkPanel {
    GtkWidget *root;
    GtkEntry *learnerId,*displayName,*projectPath,*reportPath;
    GtkDropDown *lessonSelect,*answers[UMI_EDUCATION_QUESTIONS];
    GtkLabel *heading,*explanation,*example,*practice,*hint,*questions[UMI_EDUCATION_QUESTIONS];
    GtkLabel *summary,*status,*feedback;
    GtkTextBuffer *note;
    GtkTextView *noteView;
    UmiDataServer *server;
    UmiEducationWorkspace *workspace;
    bool ownsServer,loading,dirty;
    size_t lessonIndex;
    GWeakRef guardedWindow;
    gulong closeHandler;
};
void EwGtkMessage(UmiEducationGtkPanel *panel,const char *message);
void EwGtkStatus(UmiEducationGtkPanel *panel,UmiStatus status,const char *success);
void EwGtkRefresh(UmiEducationGtkPanel *panel,bool loadNote);
void EwGtkAction(GtkButton *button,gpointer context);
#endif
