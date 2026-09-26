/* Umicom Framework | Education native adapter
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * Reusable presentation belongs in Framework; application modules stay thin. */
#include "education_workspace_private.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
