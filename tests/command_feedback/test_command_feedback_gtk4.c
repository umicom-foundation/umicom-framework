/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/command_feedback/test_command_feedback_gtk4.c
 * PURPOSE: Verify a rejected panel action is rendered after its GTK callback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/suite_layout/gtk4_product_workstation.h"
#include "umicom/application/experience_catalogue.h"
#include <stdio.h>
#include <string.h>
#define REQUIRE(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); result=1; goto cleanup; } } while(0)
static unsigned commandCalls;
static const char *failureText="This test action was refused. The pending edit remains open.";
static UmiStatus Controller(void *context,const UmiApplicationPresentationPanelPlacement *p,
    UmiApplicationPresentationSurfaceEvent event,const char *payload,
    UmiApplicationPresentationSurfaceUpdate *update)
{
    (void)context;(void)p;(void)payload;
    memset(update,0,sizeof(*update));update->state=UMI_APPLICATION_PRESENTATION_STATE_READY;update->dirty=1;
    if(event==UMI_APPLICATION_PRESENTATION_EVENT_COMMAND) {
        commandCalls++;
        return UmiApplicationPresentationSurfaceFailure(UMI_STATUS_IO_ERROR,failureText,update);
    }
    if(event==UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT || event==UMI_APPLICATION_PRESENTATION_EVENT_DEACTIVATE)
        update->state=UMI_APPLICATION_PRESENTATION_STATE_DORMANT;
    return UMI_STATUS_OK;
}
static UmiStatus Register(UmiApplicationPresentationSurfaceRuntime *runtime,void *context)
{return umi_application_presentation_surface_runtime_register_controller_for_all(runtime,Controller,context);}
static GtkWidget *Find(GtkWidget *root,const char *id,int text)
{
    GtkWidget *child;const char *value=text && GTK_IS_LABEL(root)?gtk_label_get_text(GTK_LABEL(root)):
        (!text?g_object_get_data(G_OBJECT(root),"umicom-automation-id"):NULL);
    if(value!=NULL && (text?strstr(value,id)!=NULL:strcmp(value,id)==0))return root;
    for(child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,id,text);if(found!=NULL)return found;
    }
    return NULL;
}
/* Bound each event-pump visit; an external CTest timeout supervises a hung callback. */
static void Pump(void)
{unsigned i;for(i=0;i<32U && g_main_context_pending(NULL);++i)g_main_context_iteration(NULL,FALSE);}
int main(int argc,char **argv)
{
    UmiApplicationProductGtk4Workstation *workstation=NULL;
    UmiApplicationProductGtk4WorkstationConfig config;
    UmiApplicationProductGtk4WorkstationSnapshot before,after;
    const UmiApplicationExperienceDefinition *experience;
    GtkWidget *window=NULL,*root=NULL,*button=NULL;
    char application[80];size_t i;int result=0;gint64 deadline;
    if(argc!=2)return 2;
    if(!gtk_init_check())return 77;
    snprintf(application,sizeof(application),"org.umicom.%s",argv[1]);
    experience=umi_application_experience_catalogue_find(application);REQUIRE(experience);
    config=umi_application_product_gtk4_workstation_config_default(application,"Umicom command test",Register,NULL);
    REQUIRE(umi_application_product_gtk4_workstation_create(&config,&workstation)==UMI_STATUS_OK);
    root=umi_application_product_gtk4_workstation_widget(workstation);REQUIRE(root);
    window=gtk_window_new();gtk_window_set_default_size(GTK_WINDOW(window),1200,850);
    gtk_window_set_child(GTK_WINDOW(window),root);
    REQUIRE(umi_application_product_gtk4_workstation_bind_window(workstation,GTK_WINDOW(window))==UMI_STATUS_OK);
    gtk_window_present(GTK_WINDOW(window));
    deadline=g_get_monotonic_time()+2000000;
    while(!gtk_widget_get_mapped(root) && g_get_monotonic_time()<deadline){Pump();g_usleep(1000);}
    REQUIRE(gtk_widget_get_mapped(root));
    for(i=0;i<experience->panel_count && button==NULL;++i) {
        const UmiApplicationProductPanelBinding *binding=umi_application_product_panel_binding_find(
            application,experience->panels[i].panel_id);
        if(binding && binding->command_id && binding->command_id[0]) {
            GtkWidget *candidate=Find(root,binding->command_id,0);
            if(candidate && GTK_IS_BUTTON(candidate) && gtk_widget_is_sensitive(candidate))button=candidate;
        }
    }
    REQUIRE(button);REQUIRE(umi_application_product_gtk4_workstation_snapshot(workstation,&before)==UMI_STATUS_OK);
    /* This exercises a GTK signal, not operating-system mouse injection. */
    g_signal_emit_by_name(button,"clicked");button=NULL;
    deadline=g_get_monotonic_time()+2000000;
    while(Find(root,failureText,1)==NULL && g_get_monotonic_time()<deadline){Pump();g_usleep(1000);}
    REQUIRE(commandCalls==1 && Find(root,failureText,1)!=NULL);
    REQUIRE(umi_application_product_gtk4_workstation_snapshot(workstation,&after)==UMI_STATUS_OK);
    REQUIRE(after.surface.dirty_count==before.surface.dirty_count && after.surface.attention_count>0U);
cleanup:
    if(window)gtk_window_set_child(GTK_WINDOW(window),NULL);
    umi_application_product_gtk4_workstation_destroy(workstation);
    if(window)gtk_window_destroy(GTK_WINDOW(window));
    return result;
}
