/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/enterprise_workspace_application_gtk4.c
 * PURPOSE: Keep branding and existing layouts while adding the shared enterprise panel.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/enterprise_workspace.h"
#include "umicom/application/suite_layout/gtk4_product_content_extension.h"
typedef struct EnterpriseApplication {
    UmiEnterpriseWorkspaceGtkPanel *panel;
    GtkWidget *wrapper, *original;
    GtkStack *pages;
    const char *applicationId;
} EnterpriseApplication;
/* Release callbacks and domain services before retained widgets outlive this
 * composition. The established product host owns its original layout tree. */
static void Release(void *data)
{
    EnterpriseApplication *application=data;
    if(application->original!=NULL&&application->pages!=NULL&&gtk_widget_get_parent(application->original)==GTK_WIDGET(application->pages))
        gtk_stack_remove(application->pages,application->original);
    application->original=NULL;
    UmiEnterpriseWorkspaceGtkPanelDestroy(application->panel);application->panel=NULL;
    if(application->wrapper!=NULL)g_object_unref(application->wrapper);
    application->wrapper=NULL;application->pages=NULL;
}
static UmiStatus Build(GtkWindow *window,UmiApplicationProductGtk4Workstation *workstation,
    GtkWidget *original,void *data,GtkWidget **outContent)
{
    EnterpriseApplication *application=data;(void)window;(void)workstation;*outContent=NULL;
    UmiStatus status=UmiEnterpriseWorkspaceGtkPanelCreate(NULL,NULL,application->applicationId,&application->panel);
    if(status!=UMI_STATUS_OK)return status;
    application->wrapper=gtk_box_new(GTK_ORIENTATION_VERTICAL,8);g_object_ref_sink(application->wrapper);
    application->pages=GTK_STACK(gtk_stack_new());gtk_widget_set_vexpand(GTK_WIDGET(application->pages),TRUE);
    GtkWidget *switcher=gtk_stack_switcher_new();gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher),application->pages);
    gtk_widget_set_halign(switcher,GTK_ALIGN_CENTER);gtk_box_append(GTK_BOX(application->wrapper),switcher);
    gtk_box_append(GTK_BOX(application->wrapper),GTK_WIDGET(application->pages));
    gtk_stack_add_titled(application->pages,UmiEnterpriseWorkspaceGtkPanelWidget(application->panel),"enterprise-workspace","Enterprise workspace");
    gtk_stack_add_titled(application->pages,original,"layouts","Existing layouts");
    application->original=original;*outContent=application->wrapper;return UMI_STATUS_OK;
}
int UmiEnterpriseWorkspaceGtkRun(const char *applicationId,const char *title,int argc,char **argv)
{
    EnterpriseApplication application={0};application.applicationId=applicationId;
    UmiApplicationProductGtk4WorkstationConfig config=umi_application_product_gtk4_workstation_config_default(applicationId,title,NULL,NULL);
    UmiApplicationProductGtk4ContentExtension extension={Build,Release,&application};
    config.mode_badge="Reference-data practice";
    int result=umi_application_product_gtk4_run_with_content(&config,&extension,argc,argv);
    Release(&application);return result;
}
