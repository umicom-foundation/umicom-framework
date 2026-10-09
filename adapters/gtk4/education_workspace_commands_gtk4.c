/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/education_workspace_commands_gtk4.c
 * PURPOSE:
 *   Reusable presentation belongs in Framework; application modules stay thin.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Education native adapter
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * Reusable presentation belongs in Framework; application modules stay thin. */
#include "education_workspace_private.h"
#include "umicom/education_workspace/local_record.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "education_storage_location_gtk4.inc"

/* Learning storage now opens an explicit user-selected database instead of silently creating an application-data folder. The previous location policy remains here for migration and engineering review; users can still enter an existing database path. The previous implementation is retained for engineering review. */
#if 0
static UmiStatus OpenRecord(UmiEducationGtkPanel *p)
{
    if(p->dirty)return UMI_STATUS_BUSY;
    const char *base=g_get_user_data_dir();
    if(base==NULL || !g_path_is_absolute(base))return UMI_STATUS_INVALID_STATE;
    char *folder=g_build_filename(base,"umicom","education",NULL),*path=g_build_filename(folder,"learning.sqlite",NULL);
    UmiDataServer *server=NULL;
    UmiStatus s=g_mkdir_with_parents(folder,0700)==0?umi_data_server_create_sqlite(path,&server):UMI_STATUS_IO_ERROR;
    if(s==UMI_STATUS_OK)s=UmiEducationGtkBind(p,server,gtk_editable_get_text(GTK_EDITABLE(p->learnerId)),gtk_editable_get_text(GTK_EDITABLE(p->displayName)));
    if(s==UMI_STATUS_OK){p->ownsServer=true;char *message=g_strdup_printf("Learner opened. Plaintext storage: %s",path);EwGtkMessage(p,message);g_free(message);}
    else umi_data_server_destroy(server);
    g_free(path);g_free(folder);return s;
}
#endif
/* Opening a database waits for its file chooser to finish so an unfinished location choice cannot be mistaken for the accepted path. Retain the earlier explicit-path opener for review. The previous implementation is retained for engineering review. */
#if 0
static UmiStatus OpenRecord(UmiEducationGtkPanel *p)
{
    if(p->closed)return UMI_STATUS_INVALID_STATE;
    if(p->dirty)return UMI_STATUS_BUSY;
    const char *path=gtk_editable_get_text(GTK_EDITABLE(p->storagePath));
    UmiDataServer *server=NULL;
    UmiEducationWorkspace *workspace=NULL;
    UmiStatus status=UmiEducationLocalRecordOpenAt(path,
        gtk_editable_get_text(GTK_EDITABLE(p->learnerId)),
        gtk_editable_get_text(GTK_EDITABLE(p->displayName)),&server,&workspace);
    if(status!=UMI_STATUS_OK)return status;
    /* A failed open leaves the current learner untouched. Transfer the complete
     * pair only once its saved state is readable, then release the prior owners.
     * Borrowed connections supplied by embedded hosts remain owned by that host. */
    UmiEducationWorkspace *previousWorkspace=p->workspace;
    UmiDataServer *previousServer=p->server;
    bool previousOwned=p->ownsServer;
    p->workspace=workspace;p->server=server;p->ownsServer=true;
    UmiEducationClose(previousWorkspace);
    if(previousOwned)umi_data_server_destroy(previousServer);
    /* Copy entry text before notifications can edit it. The active label names
     * the actual opened file even when the user drafts a different next path. */
    char *message=g_strdup_printf("Active plaintext learning database: %s",path);
    gtk_label_set_text(p->storageLocation,message);
    if(!p->closed)EwGtkRefresh(p,true);
    if(!p->closed)EwGtkMessage(p,message);
    g_free(message);
    return UMI_STATUS_OK;
}
#endif
static UmiStatus OpenRecord(UmiEducationGtkPanel *p)
{
    if(p->closed)return UMI_STATUS_INVALID_STATE;
    if(p->dirty || p->storageChoosing)return UMI_STATUS_BUSY;
    const char *path=gtk_editable_get_text(GTK_EDITABLE(p->storagePath));
    UmiDataServer *server=NULL;
    UmiEducationWorkspace *workspace=NULL;
    UmiStatus status=UmiEducationLocalRecordOpenAt(path,
        gtk_editable_get_text(GTK_EDITABLE(p->learnerId)),
        gtk_editable_get_text(GTK_EDITABLE(p->displayName)),&server,&workspace);
    if(status!=UMI_STATUS_OK)return status;
    /* A failed open leaves the current learner untouched. Transfer the complete
     * pair only once its saved state is readable, then release the prior owners.
     * Borrowed connections supplied by embedded hosts remain owned by that host. */
    UmiEducationWorkspace *previousWorkspace=p->workspace;
    UmiDataServer *previousServer=p->server;
    bool previousOwned=p->ownsServer;
    p->workspace=workspace;p->server=server;p->ownsServer=true;
    UmiEducationClose(previousWorkspace);
    if(previousOwned)umi_data_server_destroy(previousServer);
    /* Copy entry text before notifications can edit it. The active label names
     * the actual opened file even when the user drafts a different next path. */
    char *message=g_strdup_printf("Active plaintext learning database: %s",path);
    gtk_label_set_text(p->storageLocation,message);
    if(!p->closed)EwGtkRefresh(p,true);
    if(!p->closed)EwGtkMessage(p,message);
    g_free(message);
    return UMI_STATUS_OK;
}
static UmiStatus ExportRecord(UmiEducationGtkPanel *p)
{
    const char *path=gtk_editable_get_text(GTK_EDITABLE(p->reportPath));
    if(!g_path_is_absolute(path))return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    /* Match the project exporter: reports also require a local drive path. */
    if(!g_ascii_isalpha(path[0]) || path[1]!=':' ||
        (path[2]!='/' && path[2]!='\\'))return UMI_STATUS_INVALID_ARGUMENT;
#endif
    size_t size=0U;UmiStatus s=UmiEducationReportHtml(p->workspace,NULL,0U,&size);
    if(s!=UMI_STATUS_OK)return s;
    char *html=g_try_malloc(size);if(html==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    s=UmiEducationReportHtml(p->workspace,html,size,&size);
    GError *error=NULL;GFile *file=g_file_new_for_path(path);GFileOutputStream *stream=NULL;
    if(s==UMI_STATUS_OK){stream=g_file_create(file,G_FILE_CREATE_PRIVATE,NULL,&error);if(stream==NULL)s=g_error_matches(error,G_IO_ERROR,G_IO_ERROR_EXISTS)?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_IO_ERROR;}
    if(stream!=NULL){gsize written=0U;bool ok=g_output_stream_write_all(G_OUTPUT_STREAM(stream),html,size-1U,&written,NULL,&error)!=FALSE;
        g_clear_error(&error);if(!g_output_stream_close(G_OUTPUT_STREAM(stream),NULL,&error))ok=false;
        if(!ok){s=UMI_STATUS_IO_ERROR;}
        g_object_unref(stream);}
    g_clear_error(&error);g_object_unref(file);g_free(html);return s;
}
/* Export remains a separate user action. Its copied metadata records exactly
 * which directory was written; later entry/lesson edits cannot redirect Open. */
static void ExportCourseProject(UmiEducationGtkPanel *p,const UmiEducationLesson *lesson)
{
    p->exportedProjectReady=false;
    gtk_widget_set_sensitive(p->openProjectButton,FALSE);
    if(p->closed)return;
    if(p->exportedProject==NULL)p->exportedProject=g_try_new0(UmiEducationProjectWorkflow,1U);
    if(p->exportedProject==NULL){EwGtkStatus(p,UMI_STATUS_OUT_OF_MEMORY,"");return;}
    size_t written=0U;
    UmiStatus status=UmiEducationProjectExportForDevelopment(lesson->projectId,
        gtk_editable_get_text(GTK_EDITABLE(p->projectPath)),&written,p->exportedProject);
    if(status==UMI_STATUS_OK){
        p->exportedProjectReady=true;
        gtk_label_set_text(p->exportedProjectLabel,p->exportedProject->project.root);
        gtk_widget_set_sensitive(p->openProjectButton,p->openProject!=NULL);
    }else gtk_label_set_text(p->exportedProjectLabel,
        "Export did not complete. Keep any partial new folder for inspection; no IDE adoption is enabled.");
    char message[256];(void)snprintf(message,sizeof message,
        "Exported %zu files. Review the project, then open it in the IDE. No compiler was started.%s",
        written,status==UMI_STATUS_OK&&p->exportedProject->requires_framework_sdk
            ?" This course needs an installed Framework data SDK.":"");
    EwGtkStatus(p,status,message);
}
static void OpenExportedProject(UmiEducationGtkPanel *p)
{
    if(p->closed || !p->exportedProjectReady || p->openProject==NULL || p->exportedProject==NULL)return;
    UmiEducationProjectWorkflow *snapshot=g_try_new(UmiEducationProjectWorkflow,1U);
    if(snapshot==NULL){EwGtkStatus(p,UMI_STATUS_OUT_OF_MEMORY,"");return;}
    *snapshot=*p->exportedProject;
    UmiEducationGtkProjectOpen open=p->openProject;
    void *context=p->openProjectContext;
    /* The callback can dispose this panel or switch the host workspace. Do not
     * read the controller or its borrowed context again after invoking it. */
    open(snapshot,context);
    g_free(snapshot);
}
/* A completed course export now retains its exact IDE model and build profile for a separate Open action. Existing exports and learning actions remain available; retain the earlier routing for review. The previous implementation is retained for engineering review. */
#if 0
void EwGtkAction(GtkButton *button,gpointer context)
{
    UmiEducationGtkPanel *p=context;const char *action=g_object_get_data(G_OBJECT(button),"education-action");
    const UmiEducationLesson *l=UmiEducationLessonAt(p->lessonIndex);UmiStatus s=UMI_STATUS_OK;
    if(strcmp(action,"open")==0){s=OpenRecord(p);if(s!=UMI_STATUS_OK)EwGtkStatus(p,s,"");return;}
    if(strcmp(action,"export-project")==0){size_t written=0U;s=UmiEducationExportProject(l->projectId,gtk_editable_get_text(GTK_EDITABLE(p->projectPath)),&written);char message[180];(void)snprintf(message,sizeof message,"Exported %zu files. Open the new directory in Studio; inspect it before building. No compiler was started.",written);EwGtkStatus(p,s,message);return;}
    if(strcmp(action,"discard-note")==0){p->dirty=false;EwGtkRefresh(p,true);EwGtkMessage(p,"Unsaved note edits discarded; the saved note is shown.");return;}
    if(p->workspace==NULL){EwGtkMessage(p,"Open a learner record first.");return;}
    if(strcmp(action,"read")==0)s=UmiEducationMarkRead(p->workspace,l->id);
    else if(strcmp(action,"hint")==0)s=UmiEducationRevealHint(p->workspace,l->id);
    else if(strcmp(action,"reload")==0){if(p->dirty){EwGtkMessage(p,"Save or discard note edits before reloading.");return;}s=UmiEducationReload(p->workspace);if(s==UMI_STATUS_OK)EwGtkRefresh(p,true);}
    else if(strcmp(action,"save-note")==0){GtkTextIter begin,end;gtk_text_buffer_get_bounds(p->note,&begin,&end);char *note=gtk_text_buffer_get_text(p->note,&begin,&end,FALSE);s=UmiEducationSaveNote(p->workspace,l->id,note);g_free(note);if(s==UMI_STATUS_OK)p->dirty=false;}
    else if(strcmp(action,"quiz")==0){uint32_t answers[3];UmiEducationFeedback feedback;
        for(size_t i=0U;i<3U;++i){guint a=gtk_drop_down_get_selected(p->answers[i]);if(a<1U||a>3U){EwGtkMessage(p,"Choose an answer for each of the three questions.");return;}answers[i]=a-1U;}
        s=UmiEducationSubmitQuiz(p->workspace,l->id,answers,&feedback);
        if(s==UMI_STATUS_OK){GString *text=g_string_new(NULL);g_string_append_printf(text,"This attempt: %u/100. %s\n",feedback.score,feedback.passed?"Quiz passed.":"Read the explanations and try again.");for(size_t i=0U;i<3U;++i)g_string_append_printf(text,"\n%zu. %s %s\n",i+1U,feedback.correct[i]?"Correct.":"Review:",feedback.explanations[i]);gtk_label_set_text(p->feedback,text->str);g_string_free(text,TRUE);}
    }else if(strcmp(action,"export-record")==0){s=ExportRecord(p);EwGtkStatus(p,s,"Learning record exported. It records saved quiz activity, not compiled-code results.");return;}
    else s=UMI_STATUS_INVALID_ARGUMENT;
    if(s==UMI_STATUS_OK){EwGtkRefresh(p,false);}
    EwGtkStatus(p,s,"Learning action saved.");
}
#endif
/* A host opener can dispose its panel during an action. Acquire its controller across routing so the borrowed metadata and host context survive until the action returns. The previous implementation is retained for engineering review. */
#if 0
void EwGtkAction(GtkButton *button,gpointer context)
{
    UmiEducationGtkPanel *p=context;const char *action=g_object_get_data(G_OBJECT(button),"education-action");
    const UmiEducationLesson *l=UmiEducationLessonAt(p->lessonIndex);UmiStatus s=UMI_STATUS_OK;
    if(strcmp(action,"open")==0){s=OpenRecord(p);if(s!=UMI_STATUS_OK)EwGtkStatus(p,s,"");return;}
    if(strcmp(action,"export-project")==0){ExportCourseProject(p,l);return;}
    if(strcmp(action,"open-project")==0){OpenExportedProject(p);return;}
    if(strcmp(action,"discard-note")==0){p->dirty=false;EwGtkRefresh(p,true);EwGtkMessage(p,"Unsaved note edits discarded; the saved note is shown.");return;}
    if(p->workspace==NULL){EwGtkMessage(p,"Open a learner record first.");return;}
    if(strcmp(action,"read")==0)s=UmiEducationMarkRead(p->workspace,l->id);
    else if(strcmp(action,"hint")==0)s=UmiEducationRevealHint(p->workspace,l->id);
    else if(strcmp(action,"reload")==0){if(p->dirty){EwGtkMessage(p,"Save or discard note edits before reloading.");return;}s=UmiEducationReload(p->workspace);if(s==UMI_STATUS_OK)EwGtkRefresh(p,true);}
    else if(strcmp(action,"save-note")==0){GtkTextIter begin,end;gtk_text_buffer_get_bounds(p->note,&begin,&end);char *note=gtk_text_buffer_get_text(p->note,&begin,&end,FALSE);s=UmiEducationSaveNote(p->workspace,l->id,note);g_free(note);if(s==UMI_STATUS_OK)p->dirty=false;}
    else if(strcmp(action,"quiz")==0){uint32_t answers[3];UmiEducationFeedback feedback;
        for(size_t i=0U;i<3U;++i){guint a=gtk_drop_down_get_selected(p->answers[i]);if(a<1U||a>3U){EwGtkMessage(p,"Choose an answer for each of the three questions.");return;}answers[i]=a-1U;}
        s=UmiEducationSubmitQuiz(p->workspace,l->id,answers,&feedback);
        if(s==UMI_STATUS_OK){GString *text=g_string_new(NULL);g_string_append_printf(text,"This attempt: %u/100. %s\n",feedback.score,feedback.passed?"Quiz passed.":"Read the explanations and try again.");for(size_t i=0U;i<3U;++i)g_string_append_printf(text,"\n%zu. %s %s\n",i+1U,feedback.correct[i]?"Correct.":"Review:",feedback.explanations[i]);gtk_label_set_text(p->feedback,text->str);g_string_free(text,TRUE);}
    }else if(strcmp(action,"export-record")==0){s=ExportRecord(p);EwGtkStatus(p,s,"Learning record exported. It records saved quiz activity, not compiled-code results.");return;}
    else s=UMI_STATUS_INVALID_ARGUMENT;
    if(s==UMI_STATUS_OK){EwGtkRefresh(p,false);}
    EwGtkStatus(p,s,"Learning action saved.");
}
#endif
static void EducationAction(GtkButton *button,gpointer context)
{
    UmiEducationGtkPanel *p=context;const char *action=g_object_get_data(G_OBJECT(button),"education-action");
    const UmiEducationLesson *l=UmiEducationLessonAt(p->lessonIndex);UmiStatus s=UMI_STATUS_OK;
    if(strcmp(action,"open")==0){s=OpenRecord(p);if(s!=UMI_STATUS_OK)EwGtkStatus(p,s,"");return;}
    if(strcmp(action,"export-project")==0){ExportCourseProject(p,l);return;}
    if(strcmp(action,"open-project")==0){OpenExportedProject(p);return;}
    if(strcmp(action,"discard-note")==0){p->dirty=false;EwGtkRefresh(p,true);EwGtkMessage(p,"Unsaved note edits discarded; the saved note is shown.");return;}
    if(p->workspace==NULL){EwGtkMessage(p,"Open a learner record first.");return;}
    if(strcmp(action,"read")==0)s=UmiEducationMarkRead(p->workspace,l->id);
    else if(strcmp(action,"hint")==0)s=UmiEducationRevealHint(p->workspace,l->id);
    else if(strcmp(action,"reload")==0){if(p->dirty){EwGtkMessage(p,"Save or discard note edits before reloading.");return;}s=UmiEducationReload(p->workspace);if(s==UMI_STATUS_OK)EwGtkRefresh(p,true);}
    else if(strcmp(action,"save-note")==0){GtkTextIter begin,end;gtk_text_buffer_get_bounds(p->note,&begin,&end);char *note=gtk_text_buffer_get_text(p->note,&begin,&end,FALSE);s=UmiEducationSaveNote(p->workspace,l->id,note);g_free(note);if(s==UMI_STATUS_OK)p->dirty=false;}
    else if(strcmp(action,"quiz")==0){uint32_t answers[3];UmiEducationFeedback feedback;
        for(size_t i=0U;i<3U;++i){guint a=gtk_drop_down_get_selected(p->answers[i]);if(a<1U||a>3U){EwGtkMessage(p,"Choose an answer for each of the three questions.");return;}answers[i]=a-1U;}
        s=UmiEducationSubmitQuiz(p->workspace,l->id,answers,&feedback);
        if(s==UMI_STATUS_OK){GString *text=g_string_new(NULL);g_string_append_printf(text,"This attempt: %u/100. %s\n",feedback.score,feedback.passed?"Quiz passed.":"Read the explanations and try again.");for(size_t i=0U;i<3U;++i)g_string_append_printf(text,"\n%zu. %s %s\n",i+1U,feedback.correct[i]?"Correct.":"Review:",feedback.explanations[i]);gtk_label_set_text(p->feedback,text->str);g_string_free(text,TRUE);}
    }else if(strcmp(action,"export-record")==0){s=ExportRecord(p);EwGtkStatus(p,s,"Learning record exported. It records saved quiz activity, not compiled-code results.");return;}
    else s=UMI_STATUS_INVALID_ARGUMENT;
    if(s==UMI_STATUS_OK){EwGtkRefresh(p,false);}
    EwGtkStatus(p,s,"Learning action saved.");
}

void EwGtkAction(GtkButton *button,gpointer context)
{
    UmiEducationGtkPanel *panel=EwGtkAcquire(context);
    if(panel==NULL)return;
    if(panel->actionRunning){EwGtkRelease(panel);return;}
    panel->actionRunning=true;
    EducationAction(button,panel);
    panel->actionRunning=false;
    EwGtkRelease(panel);
}
