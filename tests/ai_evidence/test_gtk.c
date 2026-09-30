/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_evidence/test_gtk.c
 * PURPOSE:
 *   Optional real GTK widget integration, never a simulated widget library.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Optional real GTK widget integration, never a simulated widget library. */
#include "fixture.h"
#include "umicom/ui/gtk4/ai_workspace.h"
static GtkWidget *Find(GtkWidget *widget,const char *name)
{
    const char *tag=g_object_get_data(G_OBJECT(widget),"umicom-automation-id");
    if(tag!=NULL&&strcmp(tag,name)==0)return widget;
    for(GtkWidget *child=gtk_widget_get_first_child(widget);child;child=gtk_widget_get_next_sibling(child)){
        GtkWidget *found=Find(child,name);if(found)return found;
    }return NULL;
}
static void Click(GtkWidget *root,const char *name)
{
    GtkWidget *button=Find(root,name);if(button)g_signal_emit_by_name(button,"clicked");
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(!gtk_init_check())return 77;
    Fixture f;CHECK(OpenFixture(&f,NULL)==0);
    UmiAiWorkspaceCancellation *cancel=NULL;OK(UmiAiWorkspaceCancellationCreate(&cancel));
    UmiAiWorkspaceGtkPanel *panel=NULL;
    OK(UmiAiWorkspaceGtkPanelCreate(f.workspace,&f.runtime,cancel,"Memory-only fixture","org.umicom.rag",&panel));
    GtkWidget *root=UmiAiWorkspaceGtkPanelWidget(panel);
    CHECK(Find(root,"ai.job.prepare")&&Find(root,"ai.job.prepare_inspected")&&Find(root,"ai.evidence.inspect"));
    if(strcmp(argv[1],"retained_button")==0){
        GtkWidget *button=Find(root,"ai.evidence.inspect");g_object_ref(button);
        UmiAiWorkspaceGtkPanelDestroy(panel);panel=NULL;
        g_signal_emit_by_name(button,"clicked");g_object_unref(button);
    }else if(strcmp(argv[1],"selected_preparation")==0 || strcmp(argv[1],"stale_selection")==0){
        gtk_editable_set_text(GTK_EDITABLE(Find(root,"ai.collection.id")),"notes");
        gtk_editable_set_text(GTK_EDITABLE(Find(root,"ai.search.query")),"checkpoint");
        Click(root,"ai.search.run");
        if(strcmp(argv[1],"stale_selection")==0)
            OK(UmiAiWorkspacePutSource(f.workspace,"save","notes","Saving","changed",10U));
        Click(root,"ai.job.prepare_inspected");
        UmiAiWorkspaceSnapshot snap;OK(UmiAiWorkspaceSnapshotRead(f.workspace,&snap));
        CHECK(snap.jobCount==(strcmp(argv[1],"stale_selection")==0?0U:1U));
    }else {
        if(strcmp(argv[1],"pending")==0) OK(PrepareFixture(&f,"job",f.evidence,2U));
        else CHECK(RunFixture(&f)==0);
        if(strcmp(argv[1],"changed")==0)OK(UmiAiWorkspacePutSource(f.workspace,"save","notes","Saving","Changed",10U));
        gtk_editable_set_text(GTK_EDITABLE(Find(root,"ai.job.id")),"job");Click(root,"ai.job.load");
        UmiAiWorkspaceSnapshot before,after;OK(UmiAiWorkspaceSnapshotRead(f.workspace,&before));
        Click(root,"ai.evidence.inspect");OK(UmiAiWorkspaceSnapshotRead(f.workspace,&after));
        CHECK(before.revision==after.revision);
        GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root,"ai.evidence.report")));
        GtkTextIter start,end;gtk_text_buffer_get_bounds(buffer,&start,&end);
        char *text=gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
        CHECK(strstr(text,"not proof of truth")!=NULL);
        if(strcmp(argv[1],"changed")==0)CHECK(strstr(text,"Changed since preparation")!=NULL);
        g_free(text);
    }
    UmiAiWorkspaceGtkPanelDestroy(panel);UmiAiWorkspaceCancellationDestroy(cancel);CloseFixture(&f);return 0;
}
