/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_recovery/test_panel_gtk4.c
 * PURPOSE: Exercise the real browser, recovery controls and controller lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "umicom/ui/gtk4/enterprise_workspace.h"
static GtkWidget *Find(GtkWidget *widget,const char *id)
{
    const char *actual=g_object_get_data(G_OBJECT(widget),"umicom-automation-id");
    if(actual!=NULL&&strcmp(actual,id)==0)return widget;
    for(GtkWidget *child=gtk_widget_get_first_child(widget);child!=NULL;child=gtk_widget_get_next_sibling(child)){
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }
    return NULL;
}
static void Click(GtkWidget *root,const char *id)
{
    GtkWidget *widget=Find(root,id);if(widget!=NULL)g_signal_emit_by_name(widget,"clicked");
}
static char *Report(GtkWidget *root)
{
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root,"enterprise.query.report")));
    GtkTextIter first,last;gtk_text_buffer_get_bounds(buffer,&first,&last);
    return gtk_text_buffer_get_text(buffer,&first,&last,TRUE);
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *cases[]={"controls","browse_snapshot","recovery_create","invalidate_actor","retained_controls","independent_panels"};
    bool known=false;for(size_t i=0U;i<6U;++i)if(strcmp(argv[1],cases[i])==0)known=true;
    if(!known)return 2;
    (void)g_setenv("GTK_A11Y","test",TRUE);(void)g_setenv("GSETTINGS_BACKEND","memory",TRUE);
    if(!gtk_init_check())return 77;
    TestFixture f;UmiEnterpriseWorkspaceGtkPanel *panel=NULL;
    REQUIRE(TestOpen(&f,NULL)==0);REQUIRE(TestStale(&f)==0);uint64_t revision=TestRevision(&f);
    OK(UmiEnterpriseWorkspaceGtkPanelCreate(f.workspace,"Memory-only test","org.umicom.integration-studio",&panel));
    GtkWidget *root=UmiEnterpriseWorkspaceGtkPanelWidget(panel);REQUIRE(root!=NULL);
    REQUIRE(Find(root,"enterprise.query.capture")!=NULL&&Find(root,"enterprise.recovery.prepare")!=NULL);
    REQUIRE(gtk_notebook_get_n_pages(GTK_NOTEBOOK(Find(root,"enterprise.pages")))==7);
    REQUIRE(TestRevision(&f)==revision);REQUIRE(!gtk_widget_get_sensitive(Find(root,"enterprise.recovery.prepare")));
    gtk_editable_set_text(GTK_EDITABLE(Find(root,"enterprise.job")),"delivery");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"enterprise.actor")),1U);
    if(strcmp(argv[1],"browse_snapshot")==0){
        Click(root,"enterprise.query.capture");char *original=Report(root);
        REQUIRE(TestApply(&f,"later","item_id,label,quantity\na,Later count,18\n")==0);
        Click(root,"enterprise.query.next");char *stillFrozen=Report(root);REQUIRE(strcmp(original,stillFrozen)==0);
        Click(root,"enterprise.query.capture");char *fresh=Report(root);REQUIRE(strcmp(original,fresh)!=0);
        g_free(original);g_free(stillFrozen);g_free(fresh);
    }else if(strcmp(argv[1],"recovery_create")==0){
        Click(root,"enterprise.recovery.inspect");REQUIRE(gtk_widget_get_sensitive(Find(root,"enterprise.recovery.prepare")));
        gtk_editable_set_text(GTK_EDITABLE(Find(root,"enterprise.recovery.new-job")),"new-delivery");
        Click(root,"enterprise.recovery.prepare");UmiEnterpriseJob job;
        OK(UmiEnterpriseWorkspaceJobFind(f.workspace,"new-delivery",&job));REQUIRE(job.state==UMI_ENTERPRISE_JOB_REVIEW&&job.reviewer[0]=='\0');
        REQUIRE(TestRevision(&f)==revision+1U&&!gtk_widget_get_sensitive(Find(root,"enterprise.recovery.prepare")));
    }else if(strcmp(argv[1],"invalidate_actor")==0){
        Click(root,"enterprise.recovery.inspect");REQUIRE(gtk_widget_get_sensitive(Find(root,"enterprise.recovery.prepare")));
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"enterprise.actor")),2U);
        REQUIRE(!gtk_widget_get_sensitive(Find(root,"enterprise.recovery.prepare")));Click(root,"enterprise.recovery.prepare");
        REQUIRE(TestRevision(&f)==revision);
    }else if(strcmp(argv[1],"retained_controls")==0){
        Click(root,"enterprise.recovery.inspect");Click(root,"enterprise.query.capture");
        GtkWidget *one=g_object_ref(Find(root,"enterprise.recovery.prepare"));GtkWidget *two=g_object_ref(Find(root,"enterprise.query.capture"));
        UmiEnterpriseWorkspaceGtkPanelDestroy(panel);panel=NULL;
        g_signal_emit_by_name(one,"clicked");g_signal_emit_by_name(two,"clicked");
        REQUIRE(!g_signal_has_handler_pending(one,g_signal_lookup("clicked",GTK_TYPE_BUTTON),0U,TRUE));
        REQUIRE(!g_signal_has_handler_pending(two,g_signal_lookup("clicked",GTK_TYPE_BUTTON),0U,TRUE));
        REQUIRE(TestRevision(&f)==revision);g_object_unref(one);g_object_unref(two);
    }else if(strcmp(argv[1],"independent_panels")==0){
        UmiEnterpriseWorkspaceGtkPanel *second=NULL;OK(UmiEnterpriseWorkspaceGtkPanelCreate(f.workspace,"Same borrowed model","org.umicom.database-studio",&second));
        Click(root,"enterprise.recovery.inspect");
        REQUIRE(!gtk_widget_get_sensitive(Find(UmiEnterpriseWorkspaceGtkPanelWidget(second),"enterprise.recovery.prepare")));
        UmiEnterpriseWorkspaceGtkPanelDestroy(second);REQUIRE(TestRevision(&f)==revision);
    }
    UmiEnterpriseWorkspaceGtkPanelDestroy(panel);TestClose(&f);return 0;
}
