/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_panel_gtk4.c
 * PURPOSE: Check native edit routing and retained-widget teardown without disk, processes or an application loop.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/creative_workspace.h"
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if(!(condition)){fprintf(stderr,"Line %d: %s\n",__LINE__,#condition);result=1;goto cleanup;} } while(0)
static GtkWidget *Find(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(tag!=NULL&&strcmp(tag,id)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)){
        GtkWidget *found=Find(child,id);if(found!=NULL)return found;
    }
    return NULL;
}
static void Click(GtkWidget *root,const char *id){GtkWidget *button=Find(root,id);if(button!=NULL)g_signal_emit_by_name(button,"clicked");}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    (void)g_setenv("GTK_A11Y","test",TRUE);(void)g_setenv("GSETTINGS_BACKEND","memory",TRUE);
    if(!gtk_init_check())return 77;
    int result=0;UmiDataServer *server=NULL;UmiCreativeGtkPanel *panel=NULL;UmiCreativeGtkSurface *surface=NULL;
    UmiCreativeWorkspace *inspector=NULL;UmiCreativeProject *project=g_new0(UmiCreativeProject,1);
    GtkWidget *retained=NULL,*root=NULL,*original=NULL,*widget=NULL;
    CHECK(umi_data_server_create_memory(&server)==UMI_STATUS_OK);
    if(strcmp(argv[1],"surface_lifetime")==0){
        original=gtk_label_new("Existing product content");g_object_ref_sink(original);
        CHECK(UmiCreativeGtkSurfaceCreate(original,"media",&surface)==UMI_STATUS_OK);
        CHECK(gtk_widget_get_parent(original)!=NULL);
        retained=g_object_ref(UmiCreativeGtkSurfaceWidget(surface));
        UmiCreativeGtkSurfaceDestroy(surface);surface=NULL;
        CHECK(gtk_widget_get_parent(original)==NULL);
        CHECK(strcmp(gtk_label_get_text(GTK_LABEL(original)),"Existing product content")==0);
    }else{
        CHECK(UmiCreativeGtkPanelCreate(server,"media",&panel)==UMI_STATUS_OK);
        root=UmiCreativeGtkPanelWidget(panel);CHECK(umi_data_server_count(server)==0U);
        retained=g_object_ref(Find(root,"creative.project.create"));CHECK(GTK_IS_BUTTON(retained));
        if(strcmp(argv[1],"retained_controls")==0){
            UmiCreativeGtkPanelDestroy(panel);panel=NULL;g_signal_emit_by_name(retained,"clicked");CHECK(umi_data_server_count(server)==0U);
        }else if(strcmp(argv[1],"workflow")==0){
            Click(root,"creative.project.create");Click(root,"creative.project.starter");
            CHECK(UmiCreativeWorkspaceOpen(server,"workshop",&inspector)==UMI_STATUS_OK);
            CHECK(UmiCreativeWorkspaceCopy(inspector,project)==UMI_STATUS_OK);CHECK(project->elementCount==2U&&project->revision==2U);
            widget=Find(root,"creative.object.id");CHECK(GTK_IS_ENTRY(widget));gtk_editable_set_text(GTK_EDITABLE(widget),"card");
            Click(root,"creative.object.load");widget=Find(root,"creative.object.x");CHECK(GTK_IS_SPIN_BUTTON(widget));
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget),100.0);
            CHECK(UmiCreativeWorkspaceReload(inspector)==UMI_STATUS_OK);CHECK(UmiCreativeWorkspaceCopy(inspector,project)==UMI_STATUS_OK);
            CHECK(project->elements[1].x==40); /* Editing the form alone is inert. */
            Click(root,"creative.object.update");CHECK(UmiCreativeWorkspaceReload(inspector)==UMI_STATUS_OK);
            CHECK(UmiCreativeWorkspaceCopy(inspector,project)==UMI_STATUS_OK);CHECK(project->elements[1].x==100&&project->revision==3U);
            Click(root,"creative.history.undo");CHECK(UmiCreativeWorkspaceReload(inspector)==UMI_STATUS_OK);
            CHECK(UmiCreativeWorkspaceCopy(inspector,project)==UMI_STATUS_OK);CHECK(project->elements[1].x==40&&project->revision==4U);
            Click(root,"creative.history.redo");CHECK(UmiCreativeWorkspaceReload(inspector)==UMI_STATUS_OK);
            CHECK(UmiCreativeWorkspaceCopy(inspector,project)==UMI_STATUS_OK);CHECK(project->elements[1].x==100&&project->revision==5U);
        }else{result=2;}
    }
cleanup:
    UmiCreativeGtkSurfaceDestroy(surface);UmiCreativeGtkPanelDestroy(panel);
    if(retained!=NULL)g_object_unref(retained);
    if(original!=NULL)g_object_unref(original);
    UmiCreativeWorkspaceDestroy(inspector);umi_data_server_destroy(server);g_free(project);return result;
}
