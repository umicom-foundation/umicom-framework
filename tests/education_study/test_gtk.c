/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education_study/test_gtk.c
 * PURPOSE:
 *   This executable reports NOT RUN when no display is available.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT | Actual GTK lifecycle checks.
 * This executable reports NOT RUN when no display is available. */
#include "umicom/ui/gtk4/education_workspace.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"line %d: %s\n",__LINE__,#c);rc=1;goto done;}}while(0)
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(tag!=NULL&&strcmp(tag,id)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }
    return NULL;
}
static void Click(GtkWidget *button){g_signal_emit_by_name(button,"clicked");}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!gtk_init_check())return 77;
    int rc=0;UmiDataServer *server=NULL;UmiEducationGtkPanel *panel=NULL;GtkWidget *retained=NULL;
    CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    panel=UmiEducationGtkCreate();CHECK(panel!=NULL);
    CHECK(UmiEducationGtkBind(panel,server,"learner","GTK learner")==UMI_STATUS_OK);
/* Open the study section before searching for its mounted controls; keep all behavioural checks. The previous implementation remains for engineering review. */
#if 0
    GtkWidget *root=UmiEducationGtkWidget(panel),*capture=Find(root,"education.study.capture"),
#endif
    GtkWidget *root=UmiEducationGtkWidget(panel);
    GtkWidget *section=Find(root,"education.study");
    CHECK(GTK_IS_EXPANDER(section));
    gtk_expander_set_expanded(GTK_EXPANDER(section),TRUE);
    GtkWidget *capture=Find(root,"education.study.capture"),
        *next=Find(root,"education.study.next"),*search=Find(root,"education.study.search"),
        *output=Find(root,"education.study.output"),*selector=Find(root,"education.lesson");
    CHECK(capture!=NULL&&next!=NULL&&search!=NULL&&output!=NULL&&selector!=NULL);
    if(strcmp(argv[1],"capture")==0) {
        Click(capture);CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)),"Captured learner: learner")!=NULL);
        CHECK(umi_data_server_count(server)==0U);
    } else if(strcmp(argv[1],"library")==0) {
        gtk_editable_set_text(GTK_EDITABLE(search),"PCM16");Click(Find(root,"education.study.library"));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)),"CREATIVE_AUDIO.html")!=NULL);
    } else if(strcmp(argv[1],"invalidate")==0) {
        Click(capture);CHECK(gtk_widget_get_sensitive(next));
        gtk_editable_set_text(GTK_EDITABLE(search),"notes");CHECK(!gtk_widget_get_sensitive(next));
    } else if(strcmp(argv[1],"retained_control")==0) {
/* Retain a control from a collapsed pane to check disconnection of unmounted logical children. The previous implementation remains for engineering review. */
#if 0
        retained=g_object_ref(capture);UmiEducationGtkDestroy(panel);panel=NULL;Click(retained);
#endif
        retained=g_object_ref(capture);
        gtk_expander_set_expanded(GTK_EXPANDER(section),FALSE);
        UmiEducationGtkDestroy(panel);panel=NULL;Click(retained);
        CHECK(umi_data_server_count(server)==0U);
    } else if(strcmp(argv[1],"unsaved_note")==0) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(selector),3U);Click(capture);
        GtkWidget *note=Find(root,"education.note");CHECK(note!=NULL);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(note)),"unsaved",-1);
        Click(next);CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(selector))==3U);
    } else rc=2;
done:
    if(retained!=NULL)g_object_unref(retained);
    UmiEducationGtkDestroy(panel);umi_data_server_destroy(server);return rc;
}
