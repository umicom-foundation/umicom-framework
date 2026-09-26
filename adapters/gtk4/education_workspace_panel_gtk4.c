/* Umicom Framework | Education native adapter
 * Author: Sammy Hegab, Umicom Foundation. Licence: MIT.
 * Reusable presentation belongs in Framework; application modules stay thin. */
#include "education_workspace_private.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
static GtkWidget *Label(const char *text)
{
    GtkWidget *label=gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label),0.0F);gtk_label_set_wrap(GTK_LABEL(label),TRUE);
    gtk_label_set_selectable(GTK_LABEL(label),TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label),90);
    return label;
}
static void Tag(GtkWidget *widget,const char *id)
{g_object_set_data_full(G_OBJECT(widget),"umicom-automation-id",g_strdup(id),g_free);}
static GtkEntry *Entry(GtkWidget *box,const char *label,const char *initial,const char *tag)
{
    gtk_box_append(GTK_BOX(box),Label(label));GtkWidget *entry=gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(entry),initial);Tag(entry,tag);gtk_box_append(GTK_BOX(box),entry);return GTK_ENTRY(entry);
}
static void Button(GtkWidget *box,const char *label,const char *action,UmiEducationGtkPanel *panel)
{
    GtkWidget *button=gtk_button_new_with_label(label);char *tag=g_strdup_printf("education.%s",action);
    Tag(button,tag);g_free(tag);g_object_set_data_full(G_OBJECT(button),"education-action",g_strdup(action),g_free);
    g_signal_connect(button,"clicked",G_CALLBACK(EwGtkAction),panel);gtk_box_append(GTK_BOX(box),button);
}
void EwGtkMessage(UmiEducationGtkPanel *p,const char *message){gtk_label_set_text(p->status,message);}
void EwGtkStatus(UmiEducationGtkPanel *p,UmiStatus status,const char *success)
{
    if(status==UMI_STATUS_OK){EwGtkMessage(p,success);return;}
    const char *reason="The operation failed. No success was recorded.";
    switch(status){
    case UMI_STATUS_BUSY:reason="Newer progress exists, or storage is busy. Save or discard note edits, then reload before trying again.";break;
    case UMI_STATUS_INVALID_STATE:reason="The operation is not ready. Read this lesson, pass its prerequisite, or reopen storage after a recovery error.";break;
    case UMI_STATUS_CAPACITY_EXCEEDED:reason="A text or history limit was reached. Export the learning record and begin a new learner record when needed.";break;
    case UMI_STATUS_ALREADY_EXISTS:reason="That destination already exists. Choose a new file or directory; nothing is overwritten.";break;
    case UMI_STATUS_PARSE_ERROR:reason="The saved record is not valid for this catalogue. Keep the file for inspection; it was not reset.";break;
    case UMI_STATUS_INVALID_ARGUMENT:reason="Check the learner ID, UTF-8 text, all three answers and the absolute destination path.";break;
    case UMI_STATUS_NOT_IMPLEMENTED:reason="This capability is unavailable in this build. Persistent learning requires SQLite.";break;
    case UMI_STATUS_IO_ERROR:reason="Storage or export failed. A partial new export may remain. Existing learner files were not overwritten.";break;
    default:break;
    }
    char message[600];(void)snprintf(message,sizeof message,"Status %d: %s",(int)status,reason);EwGtkMessage(p,message);
}
void EwGtkRefresh(UmiEducationGtkPanel *p,bool loadNote)
{
    const UmiEducationLesson *l=UmiEducationLessonAt(p->lessonIndex);
    UmiEducationProgress progress={0};UmiEducationSnapshot snapshot={0};
    gtk_label_set_text(p->heading,l->title);gtk_label_set_text(p->explanation,l->explanation);
    gtk_label_set_text(p->example,l->example);gtk_label_set_text(p->practice,l->practice);
    if(p->workspace!=NULL){(void)UmiEducationProgressRead(p->workspace,l->id,&progress);(void)UmiEducationSnapshotRead(p->workspace,&snapshot);}
    gtk_label_set_text(p->hint,progress.hintViewed?l->hint:"Choose Show hint when you need a starting point.");
    char summary[768];
    if(p->workspace!=NULL){
        (void)snprintf(summary,sizeof summary,"Active learner: %s — %s\n%u/%u quizzes passed (%u%%) · revision %" PRIu64 " · this quiz: %u attempts, latest %u, best %u. %s",
            snapshot.learnerId,snapshot.displayName,snapshot.passedQuizzes,UMI_EDUCATION_LESSONS,snapshot.quizCompletionPercent,snapshot.revision,progress.attempts,progress.latestScore,progress.bestScore,
            progress.prerequisitesMet?"Prerequisite met.":"Pass the preceding quiz in this course first.");
    }else{(void)snprintf(summary,sizeof summary,"Browse any lesson. Open a learner record to save notes, hints and quiz attempts.");}
    gtk_label_set_text(p->summary,summary);
    gtk_text_view_set_editable(p->noteView,p->workspace!=NULL);
    if(loadNote){
        p->loading=true;gtk_text_buffer_set_text(p->note,progress.note,-1);p->dirty=false;
        for(size_t i=0U;i<UMI_EDUCATION_QUESTIONS;++i){
            gtk_label_set_text(p->questions[i],l->questions[i].prompt);
            const char *options[]={"Choose an answer",l->questions[i].choices[0],l->questions[i].choices[1],l->questions[i].choices[2],NULL};
            GtkStringList *model=gtk_string_list_new(options);
            gtk_drop_down_set_model(p->answers[i],G_LIST_MODEL(model));g_object_unref(model);
            gtk_drop_down_set_selected(p->answers[i],0U);
        }
        gtk_label_set_text(p->feedback,"");p->loading=false;
    }
}
static void NoteChanged(GtkTextBuffer *buffer,gpointer context)
{UmiEducationGtkPanel *p=context;(void)buffer;if(!p->loading)p->dirty=true;}
static void LessonChanged(GObject *object,GParamSpec *spec,gpointer context)
{
    UmiEducationGtkPanel *p=context;(void)object;(void)spec;
    if(p->loading)return;
    guint selected=gtk_drop_down_get_selected(p->lessonSelect);
    if(selected>=UMI_EDUCATION_LESSONS)return;
    if(p->dirty){p->loading=true;gtk_drop_down_set_selected(p->lessonSelect,(guint)p->lessonIndex);p->loading=false;EwGtkMessage(p,"Save or discard your learning-note edits before changing lessons.");return;}
    p->lessonIndex=(size_t)selected;EwGtkRefresh(p,true);
}
UmiEducationGtkPanel *UmiEducationGtkCreate(void)
{
    UmiEducationGtkPanel *p=g_new0(UmiEducationGtkPanel,1U);g_weak_ref_init(&p->guardedWindow,NULL);
    p->root=g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL,10));Tag(p->root,"education.workspace");
    gtk_widget_set_margin_start(p->root,18);gtk_widget_set_margin_end(p->root,18);
    gtk_widget_set_margin_top(p->root,18);gtk_widget_set_margin_bottom(p->root,18);
    /* Use the supplied Umicom asset, not a replacement drawing or stock logo. */
    const char *uri=UmiEducationLogoDataUri();const char *encoded=strchr(uri,',');
#if GTK_CHECK_VERSION(4,6,0)
    if(encoded!=NULL){gsize size=0U;guchar *png=g_base64_decode(encoded+1,&size);GBytes *bytes=g_bytes_new_take(png,size);GError *error=NULL;
        GdkTexture *texture=gdk_texture_new_from_bytes(bytes,&error);g_bytes_unref(bytes);
        if(texture!=NULL){GtkWidget *picture=gtk_picture_new_for_paintable(GDK_PAINTABLE(texture));gtk_widget_set_size_request(picture,160,40);gtk_widget_set_halign(picture,GTK_ALIGN_START);gtk_box_append(GTK_BOX(p->root),picture);g_object_unref(texture);}g_clear_error(&error);}
#else
    (void)encoded; /* The canonical application titlebar still supplies branding. */
#endif
    gtk_box_append(GTK_BOX(p->root),Label("Learning practicum: C, Assembly and Framework projects"));
    gtk_box_append(GTK_BOX(p->root),Label("Local self-study only. Quiz progress is not a compiled-code result or an authenticated certificate. Use fictional information; storage and notes are plaintext."));
    p->learnerId=Entry(p->root,"Learner ID to open (letters, numbers, hyphen or underscore)","learner","education.learner-id");
    p->displayName=Entry(p->root,"Display name for a new record","Workshop learner","education.display-name");
    Button(p->root,"Open learner record","open",p);Button(p->root,"Reload saved record","reload",p);
    p->summary=GTK_LABEL(Label(""));gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->summary));
    char *items[UMI_EDUCATION_LESSONS+1U]={0};
    for(size_t i=0U;i<UMI_EDUCATION_LESSONS;++i){const UmiEducationLesson *l=UmiEducationLessonAt(i);items[i]=g_strdup_printf("%s — %s",l->courseId,l->title);}
    p->lessonSelect=GTK_DROP_DOWN(gtk_drop_down_new_from_strings((const char *const *)items));
    for(size_t i=0U;i<UMI_EDUCATION_LESSONS;++i)g_free(items[i]);
    Tag(GTK_WIDGET(p->lessonSelect),"education.lesson");gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->lessonSelect));
    p->heading=GTK_LABEL(Label(""));gtk_widget_add_css_class(GTK_WIDGET(p->heading),"title-2");gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->heading));
    p->explanation=GTK_LABEL(Label(""));gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->explanation));
    p->example=GTK_LABEL(Label(""));gtk_widget_add_css_class(GTK_WIDGET(p->example),"monospace");gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->example));
    p->practice=GTK_LABEL(Label(""));gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->practice));
    Button(p->root,"Mark lesson read","read",p);Button(p->root,"Show hint","hint",p);
    p->hint=GTK_LABEL(Label(""));gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->hint));
    for(size_t i=0U;i<UMI_EDUCATION_QUESTIONS;++i){
        p->questions[i]=GTK_LABEL(Label(""));gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->questions[i]));
        const char *empty[]={"Choose an answer",NULL};p->answers[i]=GTK_DROP_DOWN(gtk_drop_down_new_from_strings(empty));
        char id[64];(void)snprintf(id,sizeof id,"education.answer.%zu",i);Tag(GTK_WIDGET(p->answers[i]),id);gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->answers[i]));
    }
    Button(p->root,"Check my answers","quiz",p);p->feedback=GTK_LABEL(Label(""));Tag(GTK_WIDGET(p->feedback),"education.feedback");gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->feedback));
    gtk_box_append(GTK_BOX(p->root),Label("Your learning note (up to 1,023 UTF-8 bytes)"));
    GtkWidget *view=gtk_text_view_new();p->noteView=GTK_TEXT_VIEW(view);Tag(view,"education.note");gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
    gtk_widget_set_size_request(view,-1,130);p->note=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));gtk_box_append(GTK_BOX(p->root),view);
    Button(p->root,"Save learning note","save-note",p);Button(p->root,"Discard note edits","discard-note",p);
    p->projectPath=Entry(p->root,"New project directory (absolute path; must not exist)","","education.project-path");
    Button(p->root,"Export this course project","export-project",p);
    p->reportPath=Entry(p->root,"New learning-record HTML file (absolute path; must not exist)","","education.report-path");
    Button(p->root,"Export learning record","export-record",p);
    p->status=GTK_LABEL(Label("No learning record has been opened. No compiler or student program runs inside this panel."));Tag(GTK_WIDGET(p->status),"education.status");gtk_box_append(GTK_BOX(p->root),GTK_WIDGET(p->status));
    g_signal_connect(p->lessonSelect,"notify::selected",G_CALLBACK(LessonChanged),p);
    g_signal_connect(p->note,"changed",G_CALLBACK(NoteChanged),p);EwGtkRefresh(p,true);return p;
}
GtkWidget *UmiEducationGtkWidget(UmiEducationGtkPanel *p){return p!=NULL?p->root:NULL;}
static void Disconnect(GtkWidget *widget,UmiEducationGtkPanel *p)
{
    g_signal_handlers_disconnect_by_data(widget,p);
    for(GtkWidget *child=gtk_widget_get_first_child(widget);child!=NULL;child=gtk_widget_get_next_sibling(child))Disconnect(child,p);
}
void UmiEducationGtkDestroy(UmiEducationGtkPanel *p)
{
    if(p==NULL)return;
    GObject *window=g_weak_ref_get(&p->guardedWindow);
    if(window!=NULL){if(p->closeHandler!=0U)g_signal_handler_disconnect(window,p->closeHandler);g_object_unref(window);}
    g_weak_ref_clear(&p->guardedWindow);
    g_signal_handlers_disconnect_by_data(p->note,p);Disconnect(p->root,p);
    UmiEducationClose(p->workspace);if(p->ownsServer)umi_data_server_destroy(p->server);
    g_object_unref(p->root);g_free(p);
}
static gboolean CloseGuard(GtkWindow *window,gpointer context)
{
    UmiEducationGtkPanel *p=context;(void)window;
    if(p->dirty){EwGtkMessage(p,"Save or explicitly discard the learning-note edits before closing.");return TRUE;}return FALSE;
}
void UmiEducationGtkAttachCloseGuard(UmiEducationGtkPanel *p,GtkWindow *window)
{
    if(p==NULL)return;
    GObject *old=g_weak_ref_get(&p->guardedWindow);
    if(old!=NULL){if(p->closeHandler!=0U)g_signal_handler_disconnect(old,p->closeHandler);g_object_unref(old);}
    g_weak_ref_set(&p->guardedWindow,window);p->closeHandler=window!=NULL?g_signal_connect(window,"close-request",G_CALLBACK(CloseGuard),p):0U;
}
UmiStatus UmiEducationGtkBind(UmiEducationGtkPanel *p,UmiDataServer *server,const char *id,const char *name)
{
    if(p==NULL || server==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(p->dirty)return UMI_STATUS_BUSY;
    /* Do not destroy an owned connection by rebinding that same pointer. */
    if(p->ownsServer && p->server==server)return UMI_STATUS_INVALID_ARGUMENT;
    UmiEducationWorkspace *fresh=NULL;UmiStatus s=UmiEducationOpen(server,id,name,&fresh);
    if(s!=UMI_STATUS_OK)return s;
    UmiEducationClose(p->workspace);if(p->ownsServer)umi_data_server_destroy(p->server);
    p->server=server;p->workspace=fresh;p->ownsServer=false;EwGtkRefresh(p,true);return UMI_STATUS_OK;
}
