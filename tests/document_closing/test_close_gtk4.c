/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_close_gtk4.c
 * PURPOSE: Exercise real GTK close controls and the document owner's result.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/document/local_provider.h"
#include "umicom/platform/filesystem.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define PROCESS_ID _getpid
#else
#include <unistd.h>
#define PROCESS_ID getpid
#endif
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); failed=1; goto cleanup; } } while(0)
typedef struct Result { unsigned count; UmiStatus status; UmiGtk4Adapter *detach; } Result;
static void Completed(void *context,UmiStatus status)
{
    Result *result=context; ++result->count;result->status=status;
    if(result->detach!=NULL) (void)UmiGtk4AdapterBindDocumentEditing(result->detach,NULL,NULL,NULL);
}
static GtkWidget *Find(GtkWidget *root,const char *tag,int tooltip)
{
    if(root==NULL)return NULL;
    const char *value=tooltip?gtk_widget_get_tooltip_text(root):g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(value!=NULL && strcmp(value,tag)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,tag,tooltip);if(found!=NULL)return found;
    }
    return NULL;
}
static GtkWidget *Question(void)
{
    GListModel *windows=gtk_window_get_toplevels();
    for(guint i=0U;i<g_list_model_get_n_items(windows);++i) {
        GtkWidget *window=g_list_model_get_item(windows,i);
        const char *tag=g_object_get_data(G_OBJECT(window),"umicom-automation-id");
        if(tag!=NULL && strcmp(tag,"document.close.dialog")==0)return window;
        g_object_unref(window);
    }
    return NULL;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *name=argv[1];
    if(strcmp(name,"clean")!=0 && strcmp(name,"cancel")!=0 && strcmp(name,"discard")!=0 &&
       strcmp(name,"save")!=0 && strcmp(name,"stale")!=0 && strcmp(name,"other-tab")!=0 &&
       strcmp(name,"unbind")!=0 && strcmp(name,"window-close")!=0 && strcmp(name,"complete-unbind")!=0 &&
       strcmp(name,"tab-clean")!=0 && strcmp(name,"tab-pinned")!=0 && strcmp(name,"save-busy")!=0)return 2;
    if(!gtk_init_check())return 77;
    UmiCommandRegistry *commands=NULL;UmiUiWorkbench *workbench=NULL;UmiUiApplicationShell *shell=NULL;
    UmiDocumentStore *store=NULL;UmiDocumentCoordinator *documents=NULL;UmiGtk4Adapter *adapter=NULL;
    GtkApplication *application=NULL;GtkWidget *question=NULL;GtkWidget *retained=NULL;
    char temp[UMI_PATH_CAPACITY],root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY],leaf[128],viewId[UMI_UI_ID_CAPACITY];
    char *disk=NULL;size_t diskBytes=0U;int failed=0,madeRoot=0;Result result={0};
    application=gtk_application_new("org.umicom.close.test",G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(application),NULL,NULL));
    REQUIRE(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("umicom.close.test",commands,&workbench)==UMI_STATUS_OK);
    REQUIRE(umi_ui_application_shell_create("org.umicom.close.test","Umicom Notes",workbench,&shell)==UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store)==UMI_STATUS_OK);
    UmiDocumentProvider provider=umi_document_local_provider();
    REQUIRE(umi_document_coordinator_create(store,workbench,&provider,&documents)==UMI_STATUS_OK);
    REQUIRE(umi_fs_temp_directory(temp,sizeof(temp))==UMI_STATUS_OK);
    REQUIRE(snprintf(leaf,sizeof(leaf),"umicom-close-native-%ld-%s",(long)PROCESS_ID(),name)>0);
    REQUIRE(umi_fs_join(root,sizeof(root),temp,leaf)==UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));REQUIRE(umi_fs_make_directories(root)==UMI_STATUS_OK);madeRoot=1;
    REQUIRE(umi_fs_join(path,sizeof(path),root,"notes.c")==UMI_STATUS_OK);
    REQUIRE(umi_fs_write_text(path,"original\n")==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_open(documents,path,viewId,sizeof(viewId))==UMI_STATUS_OK);
    UmiUiDocumentViewSnapshot view;
    REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),viewId,&view)==UMI_STATUS_OK);
    int clean=strcmp(name,"clean")==0 || strncmp(name,"tab-",4)==0;
    if(!clean) {view.dirty=1;REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench),&view,"saved Notes\n",12U)==UMI_STATUS_OK);}
    if(strcmp(name,"tab-clean")==0) {view.pinned=0;REQUIRE(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(workbench),&view)==UMI_STATUS_OK);}
    REQUIRE(umi_gtk4_adapter_create(application,&adapter)==UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_prepare(adapter,shell)==UMI_STATUS_OK);
    REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,documents,Completed,&result)==UMI_STATUS_OK);
    REQUIRE(UmiGtk4AdapterRequestDocumentClose(NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    if(strcmp(name,"save-busy")==0) {
        REQUIRE(UmiGtk4AdapterDocumentSaveAll(adapter,NULL,NULL)==UMI_STATUS_OK);
        REQUIRE(UmiGtk4AdapterRequestDocumentClose(adapter,NULL)==UMI_STATUS_BUSY);
        REQUIRE(umi_document_coordinator_count(documents)==1U && result.count==1U && result.status==UMI_STATUS_BUSY);
        goto disk_check;
    }
    if(strncmp(name,"tab-",4)==0) {
        GtkWidget *button=Find(umi_gtk4_adapter_native_window(adapter),"Close editor",1);
        REQUIRE(button!=NULL);retained=g_object_ref(button);
        g_signal_emit_by_name(button,"clicked");
        REQUIRE(umi_document_coordinator_count(documents)==(strcmp(name,"tab-pinned")==0?1U:0U));
        goto disk_check;
    }
    REQUIRE(UmiGtk4AdapterRequestDocumentClose(adapter,viewId)==UMI_STATUS_OK);
    if(clean) {REQUIRE(umi_document_coordinator_count(documents)==0U && result.count==1U);goto disk_check;}
    question=Question();REQUIRE(question!=NULL);
    GtkWidget *cancel=Find(question,"document.close.cancel",0);
    REQUIRE(cancel!=NULL && gtk_window_get_default_widget(GTK_WINDOW(question))==cancel);
    GtkWidget *button=Find(question,(strcmp(name,"save")==0 || strcmp(name,"other-tab")==0 ||
        strcmp(name,"complete-unbind")==0)?"document.close.save":"document.close.discard",0);
    REQUIRE(button!=NULL);retained=g_object_ref(button);
    if(strcmp(name,"stale")==0) {
        REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),viewId,&view)==UMI_STATUS_OK);
        view.dirty=1;REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench),&view,"newer Notes\n",12U)==UMI_STATUS_OK);
    }
    UmiDocumentId other=0U;
    if(strcmp(name,"other-tab")==0) {
        char next[UMI_UI_ID_CAPACITY];UmiDocumentWorkingCopySnapshot snapshot;
        REQUIRE(umi_document_coordinator_new(documents,"notes.h",next,sizeof(next))==UMI_STATUS_OK);
        REQUIRE(umi_document_coordinator_active_snapshot(documents,&snapshot)==UMI_STATUS_OK);other=snapshot.document_id;
    }
    if(strcmp(name,"unbind")==0) {
        REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,NULL,NULL,NULL)==UMI_STATUS_OK);
        g_signal_emit_by_name(retained,"clicked");
        REQUIRE(result.count==0U && umi_document_coordinator_count(documents)==1U);
    } else {
        if(strcmp(name,"complete-unbind")==0)result.detach=adapter;
        if(strcmp(name,"cancel")==0)g_signal_emit_by_name(cancel,"clicked");
        else if(strcmp(name,"window-close")==0)gtk_window_close(GTK_WINDOW(question));
        else g_signal_emit_by_name(retained,"clicked");
        REQUIRE(result.count==1U);
        UmiStatus expected=(strcmp(name,"cancel")==0 || strcmp(name,"window-close")==0)?UMI_STATUS_CANCELLED:
            strcmp(name,"stale")==0?UMI_STATUS_INVALID_STATE:UMI_STATUS_OK;
        REQUIRE(result.status==expected);
        if(strcmp(name,"other-tab")==0) {
            UmiDocumentWorkingCopySnapshot snapshot;REQUIRE(umi_document_coordinator_active_snapshot(documents,&snapshot)==UMI_STATUS_OK);
            REQUIRE(snapshot.document_id==other && snapshot.dirty);
        } else REQUIRE(umi_document_coordinator_count(documents)==(expected==UMI_STATUS_OK?0U:1U));
    }
disk_check:;
    REQUIRE(umi_fs_read_text(path,&disk,&diskBytes)==UMI_STATUS_OK);
    const char *expected=(strcmp(name,"save")==0 || strcmp(name,"other-tab")==0 || strcmp(name,"complete-unbind")==0)?"saved Notes\n":"original\n";
    REQUIRE(diskBytes==strlen(expected) && memcmp(disk,expected,diskBytes)==0);
cleanup:
    free(disk);
    if(adapter!=NULL)(void)UmiGtk4AdapterBindDocumentEditing(adapter,NULL,NULL,NULL);
    if(question!=NULL)gtk_window_destroy(GTK_WINDOW(question));
    g_clear_object(&retained);g_clear_object(&question);
    if(adapter!=NULL)umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);umi_ui_workbench_destroy(workbench);umi_command_registry_destroy(commands);
    g_clear_object(&application);
    if(madeRoot)(void)umi_fs_remove_tree(root);
    return failed;
}
