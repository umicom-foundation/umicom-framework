/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_editing/test_document_navigation_gtk4.c
 * PURPOSE: Check real GTK editing-result delivery and the source-location form.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); failed=1; goto cleanup; } } while(0)
typedef struct Result { unsigned count; UmiStatus status; UmiGtk4Adapter *adapter; int detach; } Result;
static void Completed(void *data,UmiStatus status)
{
    Result *result=data;result->count++;result->status=status;
    if(result->detach)(void)UmiGtk4AdapterBindDocumentEditing(result->adapter,NULL,NULL,NULL);
}
static void Pump(void)
{ for(unsigned i=0;i<32 && g_main_context_pending(NULL);++i)(void)g_main_context_iteration(NULL,FALSE); }
static GtkWidget *Tagged(GtkWidget *root,const char *id)
{
    const char *tag=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(tag&&strcmp(tag,id)==0)return root;
    for(GtkWidget *c=gtk_widget_get_first_child(root);c;c=gtk_widget_get_next_sibling(c)) {
        GtkWidget *found=Tagged(c,id);if(found)return found;
    }
    return NULL;
}
/* Return a retained form; the test releases it during cleanup. */
static GtkWindow *FindForm(void)
{
    GListModel *windows=gtk_window_get_toplevels();
    for(guint i=0;i<g_list_model_get_n_items(windows);++i) {
        GtkWindow *w=g_list_model_get_item(windows,i);
        if(Tagged(GTK_WIDGET(w),"document.location.window"))return w;
        g_object_unref(w);
    }
    return NULL;
}
int main(int argc,char **argv)
{
    int failed=0;
    UmiCommandRegistry *commands=NULL;UmiUiWorkbench *workbench=NULL;
    UmiUiApplicationShell *shell=NULL;UmiDocumentStore *store=NULL;
    UmiDocumentCoordinator *documents=NULL;UmiGtk4Adapter *adapter=NULL;
    GtkApplication *application=NULL;GtkWindow *form=NULL;GtkWidget *retained=NULL;
    char viewId[UMI_UI_ID_CAPACITY];Result result={0};
    if(argc!=2)return 2;
    if(!gtk_init_check())return 77;
    application=gtk_application_new("org.umicom.navigation.test",G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(application),NULL,NULL));
    REQUIRE(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("navigation.gtk",commands,&workbench)==UMI_STATUS_OK);
    REQUIRE(umi_ui_application_shell_create("org.umicom.navigation.test","Umicom Notes",workbench,&shell)==UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store)==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_create(store,workbench,NULL,&documents)==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_new(documents,"notes.c",viewId,sizeof viewId)==UMI_STATUS_OK);
    UmiUiDocumentViewModel *views=umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot before,after;
    REQUIRE(umi_ui_document_view_model_find(views,viewId,&before)==UMI_STATUS_OK);
    const char *text="Umicom Notes\nsecond\n";before.dirty=1;before.cursor_offset=1;before.selection_length=3;
    REQUIRE(UmiUiDocumentViewModelUpsertText(views,&before,text,strlen(text))==UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_create(application,&adapter)==UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_prepare(adapter,shell)==UMI_STATUS_OK);result.adapter=adapter;
    REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,documents,Completed,&result)==UMI_STATUS_OK);
    if(strcmp(argv[1],"unknown-command")==0) {
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,"edit.unknown")==UMI_STATUS_NOT_FOUND);
        REQUIRE(result.count==1&&result.status==UMI_STATUS_NOT_FOUND);goto cleanup;
    }
    if(strcmp(argv[1],"null-command")==0) {
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(result.count==1&&result.status==UMI_STATUS_INVALID_ARGUMENT);goto cleanup;
    }
    if(strcmp(argv[1],"missing-document")==0) {
        REQUIRE(umi_document_coordinator_close_active(documents,1)==UMI_STATUS_OK);
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,"edit.undo")==UMI_STATUS_NOT_FOUND);
        REQUIRE(result.count==1&&result.status==UMI_STATUS_NOT_FOUND);goto cleanup;
    }
    if(strcmp(argv[1],"completion-detach")==0) {
        result.detach=1;
        REQUIRE(UmiGtk4AdapterDocumentNavigate(adapter,"2:2")==UMI_STATUS_OK);
        REQUIRE(result.count==1&&result.status==UMI_STATUS_OK);
        REQUIRE(!UmiGtk4AdapterDocumentHasCompletion(adapter));goto cleanup;
    }
    if(strcmp(argv[1],"direct-navigation")==0 || strcmp(argv[1],"invalid-location")==0) {
        int invalid=strcmp(argv[1],"invalid-location")==0;
        REQUIRE(UmiGtk4AdapterDocumentNavigate(adapter,invalid?"-1":"2:2")==
            (invalid?UMI_STATUS_INVALID_ARGUMENT:UMI_STATUS_OK));
        REQUIRE(result.count==1);REQUIRE(umi_ui_document_view_model_find(views,viewId,&after)==UMI_STATUS_OK);
        REQUIRE(after.cursor_offset==(invalid?1U:14U));goto cleanup;
    }
    REQUIRE(UmiGtk4AdapterPromptDocumentLocation(adapter)==UMI_STATUS_OK);Pump();
    form=FindForm();REQUIRE(form!=NULL);
    GtkWidget *entry=Tagged(GTK_WIDGET(form),"document.location.input");
    GtkWidget *go=Tagged(GTK_WIDGET(form),"document.location.go");
    GtkWidget *cancel=Tagged(GTK_WIDGET(form),"document.location.cancel");
    REQUIRE(entry&&go&&cancel);
    if(strcmp(argv[1],"form-cancel")==0) {
        g_signal_emit_by_name(cancel,"clicked");REQUIRE(result.count==0);
        REQUIRE(umi_ui_document_view_model_find(views,viewId,&after)==UMI_STATUS_OK);
        REQUIRE(after.cursor_offset==1&&after.selection_length==3);goto cleanup;
    }
    if(strcmp(argv[1],"form-rebind")==0 || strcmp(argv[1],"retained-control")==0) {
        retained=g_object_ref(go);
        if(strcmp(argv[1],"form-rebind")==0) {
            REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,documents,Completed,&result)==UMI_STATUS_OK);
            REQUIRE(UmiGtk4AdapterPromptDocumentLocation(adapter)==UMI_STATUS_OK);
        } else g_signal_emit_by_name(cancel,"clicked");
        g_signal_emit_by_name(retained,"clicked");REQUIRE(result.count==0);
        REQUIRE(umi_ui_document_view_model_find(views,viewId,&after)==UMI_STATUS_OK);
        REQUIRE(after.cursor_offset==1);goto cleanup;
    }
    if(strcmp(argv[1],"form-other-tab")==0) {
        char other[UMI_UI_ID_CAPACITY];REQUIRE(umi_document_coordinator_new(documents,"review.txt",other,sizeof other)==UMI_STATUS_OK);
        gtk_editable_set_text(GTK_EDITABLE(entry),"2");g_signal_emit_by_name(go,"clicked");
        REQUIRE(result.count==1&&result.status==UMI_STATUS_INVALID_STATE);
        REQUIRE(umi_ui_document_view_model_find(views,viewId,&after)==UMI_STATUS_OK);
        REQUIRE(after.cursor_offset==1&&after.selection_length==3);goto cleanup;
    }
    if(strcmp(argv[1],"form-enter")==0 || strcmp(argv[1],"form-go")==0) {
        gtk_editable_set_text(GTK_EDITABLE(entry),"2:2");
        if(strcmp(argv[1],"form-enter")==0)g_signal_emit_by_name(entry,"activate");
        else g_signal_emit_by_name(go,"clicked");
        REQUIRE(result.count==1&&result.status==UMI_STATUS_OK);
        REQUIRE(umi_ui_document_view_model_find(views,viewId,&after)==UMI_STATUS_OK);
        REQUIRE(after.cursor_offset==14&&after.selection_length==0);goto cleanup;
    }
    fprintf(stderr,"Unknown native test case: %s\n",argv[1]);failed=1;
cleanup:
    if(adapter)(void)UmiGtk4AdapterBindDocumentEditing(adapter,NULL,NULL,NULL);
    if(form){gtk_window_destroy(form);g_object_unref(form);}
    if(retained)g_object_unref(retained);
    Pump();umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);if(application)g_object_unref(application);
    return failed;
}
