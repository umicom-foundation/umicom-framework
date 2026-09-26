/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_workspace/test_panel_gtk4.c
 * PURPOSE: Exercise the actual unpresented learning panel and retained controls.
 * AUTHOR: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/education_workspace.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Line %d: %s\n",__LINE__,#x); failed=1; goto cleanup; } } while(0)
static GtkWidget *Find(GtkWidget *root,const char *tag)
{
    const char *value=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(value!=NULL && strcmp(value,tag)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)){
        GtkWidget *found=Find(child,tag);if(found!=NULL)return found;
    }
    return NULL;
}
static void Click(GtkWidget *root,const char *id)
{g_signal_emit_by_name(Find(root,id),"clicked");}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    (void)g_setenv("GTK_A11Y","test",TRUE);
    (void)g_setenv("GSETTINGS_BACKEND","memory",TRUE);
    if(!gtk_init_check())return 77;
    int failed=0;UmiDataServer *server=NULL;UmiEducationWorkspace *reader=NULL;
    UmiEducationGtkPanel *panel=UmiEducationGtkCreate();
    GtkWidget *retained=NULL;GtkTextBuffer *buffer=NULL;GtkWindow *window=NULL;
    CHECK(panel!=NULL);
    GtkWidget *root=UmiEducationGtkWidget(panel);
    CHECK(Find(root,"education.workspace")==root);
    CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(Find(root,"education.note"))));
    CHECK(UmiEducationGtkBind(panel,server,"practice","Workshop learner")==UMI_STATUS_OK);
    CHECK(gtk_text_view_get_editable(GTK_TEXT_VIEW(Find(root,"education.note"))));
    if(strcmp(argv[1],"workflow")==0){
        Click(root,"education.read");
        const UmiEducationLesson *lesson=UmiEducationLessonAt(0U);
        for(size_t i=0U;i<3U;++i){char tag[64];snprintf(tag,sizeof tag,"education.answer.%zu",i);
            GtkWidget *answer=Find(root,tag);CHECK(GTK_IS_DROP_DOWN(answer));
            gtk_drop_down_set_selected(GTK_DROP_DOWN(answer),lesson->questions[i].correctChoice+1U);}
        Click(root,"education.quiz");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root,"education.feedback"))),"100/100")!=NULL);
        CHECK(UmiEducationOpen(server,"practice","ignored",&reader)==UMI_STATUS_OK);
        UmiEducationSnapshot snapshot;
        CHECK(UmiEducationSnapshotRead(reader,&snapshot)==UMI_STATUS_OK && snapshot.passedQuizzes==1U);
    }else if(strcmp(argv[1],"unsaved_note")==0){
        buffer=g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root,"education.note"))));
        gtk_text_buffer_set_text(buffer,"Keep this draft",-1);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"education.lesson")),1U);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(Find(root,"education.lesson")))==0U);
        window=GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
        UmiEducationGtkAttachCloseGuard(panel,window);
        gboolean stopped=FALSE;g_signal_emit_by_name(window,"close-request",&stopped);CHECK(stopped);
        CHECK(UmiEducationGtkBind(panel,server,"other","Other learner")==UMI_STATUS_BUSY);
        Click(root,"education.save-note");
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"education.lesson")),1U);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(Find(root,"education.lesson")))==1U);
        CHECK(UmiEducationOpen(server,"practice","ignored",&reader)==UMI_STATUS_OK);
        UmiEducationProgress progress;
        CHECK(UmiEducationProgressRead(reader,"notes.values",&progress)==UMI_STATUS_OK);
        CHECK(strcmp(progress.note,"Keep this draft")==0);
    }else if(strcmp(argv[1],"retained_controls")==0){
        retained=g_object_ref(Find(root,"education.read"));
        buffer=g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root,"education.note"))));
        CHECK(g_signal_handler_find(retained,G_SIGNAL_MATCH_DATA,0U,0U,NULL,NULL,panel)!=0U);
        UmiEducationGtkDestroy(panel);panel=NULL;
        /* Old controls remain inert after their controller has gone. */
        g_signal_emit_by_name(retained,"clicked");gtk_text_buffer_set_text(buffer,"Retained buffer",-1);
        CHECK(UmiEducationOpen(server,"practice","ignored",&reader)==UMI_STATUS_OK);
        UmiEducationSnapshot snapshot;CHECK(UmiEducationSnapshotRead(reader,&snapshot)==UMI_STATUS_OK);
        CHECK(snapshot.revision==0U);
    }else{failed=1;}
cleanup:
    UmiEducationClose(reader);UmiEducationGtkDestroy(panel);
    if(window!=NULL){gtk_window_destroy(window);g_object_unref(window);}
    if(buffer!=NULL)g_object_unref(buffer);
    if(retained!=NULL)g_object_unref(retained);
    umi_data_server_destroy(server);return failed;
}
