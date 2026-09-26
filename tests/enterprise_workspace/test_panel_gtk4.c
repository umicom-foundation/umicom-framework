/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/enterprise_workspace/test_panel_gtk4.c
 * PURPOSE: Exercise the actual enterprise form bindings and retained-control teardown.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/enterprise_workspace.h"
#include "umicom/enterprise_workspace/practice.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do {if(!(x)){fprintf(stderr,"Line %d: %s\n",__LINE__,#x);failed=1;goto cleanup;}} while(0)
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
int main(int argc,char **argv)
{
    UmiDataServer *data=NULL;UmiEnterprisePracticeAccess *access=NULL;UmiEnterpriseWorkspace *workspace=NULL;
    UmiEnterpriseWorkspaceGtkPanel *panel=NULL;GtkWidget *root=NULL,*retained=NULL;int failed=0;
    UmiEnterpriseJob job;UmiEnterpriseRow row;UmiEnterpriseSnapshot before,after;
    (void)g_setenv("GTK_A11Y","test",TRUE);(void)g_setenv("GSETTINGS_BACKEND","memory",TRUE);
    if(!gtk_init_check())return 77;
    REQUIRE(UmiEnterprisePracticeAccessCreate(&access)==UMI_STATUS_OK);
    REQUIRE(umi_data_server_create_memory(&data)==UMI_STATUS_OK);
    REQUIRE(UmiEnterpriseWorkspaceOpen(data,UmiEnterprisePracticeAuthorisation(access),&workspace)==UMI_STATUS_OK);
    REQUIRE(UmiEnterpriseWorkspaceGtkPanelCreate(workspace,"Memory-only test","org.umicom.integration-studio",&panel)==UMI_STATUS_OK);
    root=UmiEnterpriseWorkspaceGtkPanelWidget(panel);REQUIRE(root!=NULL);
    REQUIRE(Find(root,"enterprise.actor")!=NULL&&Find(root,"enterprise.enable")!=NULL&&Find(root,"enterprise.apply")!=NULL);
    REQUIRE(umi_data_server_count(data)==0U); /* Construction is read-only. */
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"enterprise.actor")),4U);Click(root,"enterprise.enable");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"enterprise.actor")),1U);Click(root,"enterprise.create");Click(root,"enterprise.prepare");
    REQUIRE(UmiEnterpriseWorkspaceJobFind(workspace,"stock-opening",&job)==UMI_STATUS_OK&&job.state==UMI_ENTERPRISE_JOB_REVIEW);
    GtkTextBuffer *csv=gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root,"enterprise.csv")));
    gtk_text_buffer_set_text(csv,"item_id,label,quantity\nnotebooks,Changed after preparation,999\n",-1);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"enterprise.actor")),2U);Click(root,"enterprise.approve");
    REQUIRE(UmiEnterpriseWorkspaceJobFind(workspace,"stock-opening",&job)==UMI_STATUS_OK&&job.state==UMI_ENTERPRISE_JOB_REVIEW);
    Click(root,"enterprise.load-job");Click(root,"enterprise.approve");
    REQUIRE(UmiEnterpriseWorkspaceJobFind(workspace,"stock-opening",&job)==UMI_STATUS_OK&&job.state==UMI_ENTERPRISE_JOB_APPROVED);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root,"enterprise.actor")),3U);Click(root,"enterprise.load-job");Click(root,"enterprise.apply");
    REQUIRE(UmiEnterpriseWorkspaceRowAt(workspace,"supplies",0U,&row)==UMI_STATUS_OK&&row.quantity==12U);
    REQUIRE(strcmp(row.label,"Notebooks")==0&&strcmp(row.sourceJob,"stock-opening")==0);
    REQUIRE(UmiEnterpriseWorkspaceSnapshot(workspace,&before)==UMI_STATUS_OK);
    if(argc>1&&strcmp(argv[1],"retained_controls")==0){
        retained=g_object_ref(Find(root,"enterprise.apply"));
        UmiEnterpriseWorkspaceGtkPanelDestroy(panel);panel=NULL;
        g_signal_emit_by_name(retained,"clicked");
        REQUIRE(!g_signal_has_handler_pending(retained,g_signal_lookup("clicked",GTK_TYPE_BUTTON),0U,TRUE));
        REQUIRE(UmiEnterpriseWorkspaceSnapshot(workspace,&after)==UMI_STATUS_OK&&after.revision==before.revision);
    }
cleanup:
    UmiEnterpriseWorkspaceGtkPanelDestroy(panel);g_clear_object(&retained);
    UmiEnterpriseWorkspaceDestroy(workspace);umi_data_server_destroy(data);UmiEnterprisePracticeAccessDestroy(access);
    return failed;
}
