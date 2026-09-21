/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_close_session_gtk4.c
 * PURPOSE:
 *   Exercise real GTK group-close controls against production documents.
 *   The link case needs no display; graphical cases explicitly skip without one.
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
typedef struct Result {
    unsigned count;
    UmiDocumentCloseProgress progress;
    UmiGtk4Adapter *detach;
} Result;
static void Completed(void *context,const UmiDocumentCloseProgress *progress)
{
    Result *result=context;
    ++result->count;
    result->progress=*progress;
    if(result->detach!=NULL)(void)UmiGtk4AdapterBindDocumentEditing(result->detach,NULL,NULL,NULL);
}
static GtkWidget *Find(GtkWidget *root,const char *tag)
{
    if(root==NULL)return NULL;
    const char *value=g_object_get_data(G_OBJECT(root),"umicom-automation-id");
    if(value!=NULL && strcmp(value,tag)==0)return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child!=NULL;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=Find(child,tag);if(found!=NULL)return found;
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
/* Bounded wait: a scenario must not pass merely because no work was driven. */
static int Pump(UmiGtk4Adapter *adapter,int allowQuestion)
{
    gint64 deadline=g_get_monotonic_time()+3000000;
    while(UmiGtk4AdapterCloseDocumentsBusy(adapter)) {
        if(allowQuestion) {GtkWidget *q=Question();if(q!=NULL){g_object_unref(q);return 1;}}
        if(g_get_monotonic_time()>=deadline)return 0;
        (void)g_main_context_iteration(NULL,FALSE);
        g_usleep(1000U);
    }
    return 1;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *cases[]={"link","empty","clean","others","cancel-queued","unbind-queued","cancel-question","cancel-command","discard","save","stale","unbind-question","complete-unbind","window-destroy","save-busy","single-busy"};
    int known=0;
    for(size_t index=0U;index<sizeof(cases)/sizeof(cases[0]);++index)
        if(strcmp(argv[1],cases[index])==0)known=1;
    if(!known)return 2;
    const char *name=argv[1];
    if(strcmp(name,"link")==0) {
        UmiDocumentCloseProgress p={0};p.total=79U;
        if(UmiGtk4AdapterCloseDocuments(NULL,UMI_DOCUMENT_CLOSE_ALL,NULL,NULL)!=UMI_STATUS_INVALID_ARGUMENT)return 1;
        if(UmiGtk4AdapterCancelCloseDocuments(NULL)!=UMI_STATUS_INVALID_ARGUMENT)return 1;
        if(UmiGtk4AdapterCloseDocumentsProgress(NULL,&p)!=UMI_STATUS_INVALID_ARGUMENT || p.total!=79U)return 1;
        if(UmiGtk4AdapterCloseDocumentsBusy(NULL) || UmiGtk4AdapterDocumentCloseBusy(NULL))return 1;
        return 0;
    }
    if(!gtk_init_check())return 77;
    UmiCommandRegistry *commands=NULL;UmiUiWorkbench *workbench=NULL;UmiUiApplicationShell *shell=NULL;
    UmiDocumentStore *store=NULL;UmiDocumentCoordinator *documents=NULL;UmiGtk4Adapter *adapter=NULL;
    GtkApplication *application=NULL;GtkWidget *question=NULL,*retained=NULL;
    char temp[UMI_PATH_CAPACITY],root[UMI_PATH_CAPACITY],path[2][UMI_PATH_CAPACITY],leaf[128];
    char views[2][UMI_UI_ID_CAPACITY];UmiDocumentId ids[2]={0};
    char *disk=NULL;size_t diskBytes=0U;int failed=0,madeRoot=0;Result result={0};
    application=gtk_application_new("org.umicom.close.sequence.test",G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(application),NULL,NULL));
    REQUIRE(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("umicom.close.sequence.test",commands,&workbench)==UMI_STATUS_OK);
    REQUIRE(umi_ui_application_shell_create("org.umicom.close.sequence.test","Umicom Notes",workbench,&shell)==UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store)==UMI_STATUS_OK);
    UmiDocumentProvider provider=umi_document_local_provider();
    REQUIRE(umi_document_coordinator_create(store,workbench,&provider,&documents)==UMI_STATUS_OK);
    REQUIRE(umi_fs_temp_directory(temp,sizeof(temp))==UMI_STATUS_OK);
    REQUIRE(snprintf(leaf,sizeof(leaf),"umicom-close-group-native-%ld-%s",(long)PROCESS_ID(),name)>0);
    REQUIRE(umi_fs_join(root,sizeof(root),temp,leaf)==UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));REQUIRE(umi_fs_make_directories(root)==UMI_STATUS_OK);madeRoot=1;
    int empty=strcmp(name,"empty")==0;
    for(size_t index=0U;!empty && index<2U;++index) {
        REQUIRE(umi_fs_join(path[index],sizeof(path[index]),root,index==0U?"notes.c":"notes.h")==UMI_STATUS_OK);
        REQUIRE(umi_fs_write_text(path[index],"original\n")==UMI_STATUS_OK);
        REQUIRE(umi_document_coordinator_open(documents,path[index],views[index],sizeof(views[index]))==UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot snapshot;
        REQUIRE(umi_document_coordinator_active_snapshot(documents,&snapshot)==UMI_STATUS_OK);ids[index]=snapshot.document_id;
    }
    int dirty=!empty && strcmp(name,"clean")!=0 && strcmp(name,"cancel-queued")!=0;
    UmiUiDocumentViewSnapshot view;
    if(dirty) {
        REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),views[1],&view)==UMI_STATUS_OK);
        view.dirty=1;
        REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench),&view,"changed Notes\n",14U)==UMI_STATUS_OK);
    }
    REQUIRE(umi_gtk4_adapter_create(application,&adapter)==UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_prepare(adapter,shell)==UMI_STATUS_OK);
    REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,documents,NULL,NULL)==UMI_STATUS_OK);
    if(strcmp(name,"save-busy")==0) {
        REQUIRE(UmiGtk4AdapterDocumentSaveAll(adapter,NULL,NULL)==UMI_STATUS_OK);
        REQUIRE(UmiGtk4AdapterCloseDocuments(adapter,UMI_DOCUMENT_CLOSE_ALL,Completed,&result)==UMI_STATUS_BUSY);
        REQUIRE(umi_document_coordinator_count(documents)==2U && result.count==0U);goto cleanup;
    }
    if(strcmp(name,"single-busy")==0) {
        REQUIRE(UmiGtk4AdapterRequestDocumentClose(adapter,views[1])==UMI_STATUS_OK);
        question=Question();REQUIRE(question!=NULL);
        REQUIRE(UmiGtk4AdapterCloseDocuments(adapter,UMI_DOCUMENT_CLOSE_ALL,Completed,&result)==UMI_STATUS_BUSY);
        REQUIRE(UmiGtk4AdapterDocumentSaveAll(adapter,NULL,NULL)==UMI_STATUS_BUSY);goto cleanup;
    }
    UmiDocumentCloseScope scope=strcmp(name,"others")==0?UMI_DOCUMENT_CLOSE_OTHERS:UMI_DOCUMENT_CLOSE_ALL;
    REQUIRE(UmiGtk4AdapterCloseDocuments(adapter,scope,Completed,&result)==UMI_STATUS_OK);
    REQUIRE(UmiGtk4AdapterCloseDocumentsBusy(adapter));
    REQUIRE(UmiGtk4AdapterCloseDocuments(adapter,scope,Completed,&result)==UMI_STATUS_BUSY);
    REQUIRE(UmiGtk4AdapterDocumentSaveAll(adapter,NULL,NULL)==UMI_STATUS_BUSY);
    UmiDocumentCloseProgress progress;
    REQUIRE(UmiGtk4AdapterCloseDocumentsProgress(adapter,&progress)==UMI_STATUS_OK);
    REQUIRE(progress.total==(empty?0U:scope==UMI_DOCUMENT_CLOSE_OTHERS?1U:2U));
    if(strcmp(name,"cancel-queued")==0) {
        REQUIRE(UmiGtk4AdapterCancelCloseDocuments(adapter)==UMI_STATUS_OK);
        REQUIRE(Pump(adapter,0));REQUIRE(result.count==1U && result.progress.phase==UMI_DOCUMENT_CLOSE_CANCELLED);
        REQUIRE(umi_document_coordinator_count(documents)==2U);goto cleanup;
    }
    if(strcmp(name,"unbind-queued")==0) {
        REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,NULL,NULL,NULL)==UMI_STATUS_OK);
        REQUIRE(Pump(adapter,0));REQUIRE(result.count==0U && umi_document_coordinator_count(documents)==2U);goto cleanup;
    }
    REQUIRE(Pump(adapter,1));
    if(empty || strcmp(name,"clean")==0 || strcmp(name,"others")==0) {
        REQUIRE(result.count==1U && result.progress.phase==UMI_DOCUMENT_CLOSE_COMPLETE);
        REQUIRE(umi_document_coordinator_count(documents)==(strcmp(name,"others")==0?1U:0U));
        if(strcmp(name,"others")==0){UmiDocumentWorkingCopySnapshot current;REQUIRE(umi_document_coordinator_active_snapshot(documents,&current)==UMI_STATUS_OK && current.document_id==ids[1] && current.dirty);}
        goto cleanup;
    }
    question=Question();REQUIRE(question!=NULL);
    REQUIRE(Find(question,"document.close.progress")!=NULL);
    GtkWidget *cancel=Find(question,"document.close.cancel");
    GtkWidget *button=Find(question,strcmp(name,"save")==0?"document.close.save":"document.close.discard");
    REQUIRE(cancel!=NULL && button!=NULL && gtk_window_get_default_widget(GTK_WINDOW(question))==cancel);
    retained=g_object_ref(button);
    if(strcmp(name,"stale")==0) {
        REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),views[1],&view)==UMI_STATUS_OK);
        view.dirty=1;
        REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench),&view,"newer Notes\n",12U)==UMI_STATUS_OK);
    }
    if(strcmp(name,"complete-unbind")==0)result.detach=adapter;
    if(strcmp(name,"unbind-question")==0) {
        REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,NULL,NULL,NULL)==UMI_STATUS_OK);
        g_signal_emit_by_name(retained,"clicked");REQUIRE(result.count==0U);
        REQUIRE(umi_document_coordinator_count(documents)==1U);goto cleanup;
    } else if(strcmp(name,"window-destroy")==0)gtk_window_destroy(GTK_WINDOW(question));
    else if(strcmp(name,"cancel-question")==0)g_signal_emit_by_name(cancel,"clicked");
    else if(strcmp(name,"cancel-command")==0)REQUIRE(UmiGtk4AdapterCancelCloseDocuments(adapter)==UMI_STATUS_OK);
    else g_signal_emit_by_name(button,"clicked");
    REQUIRE(Pump(adapter,0));REQUIRE(result.count==1U);
    UmiDocumentClosePhase expected=(strncmp(name,"cancel",6)==0 || strcmp(name,"window-destroy")==0)?UMI_DOCUMENT_CLOSE_CANCELLED:
        strcmp(name,"stale")==0?UMI_DOCUMENT_CLOSE_FAILED:UMI_DOCUMENT_CLOSE_COMPLETE;
    REQUIRE(result.progress.phase==expected && result.progress.closed==(expected==UMI_DOCUMENT_CLOSE_COMPLETE?2U:1U));
    REQUIRE(umi_document_coordinator_count(documents)==(expected==UMI_DOCUMENT_CLOSE_COMPLETE?0U:1U));
    g_signal_emit_by_name(retained,"clicked");REQUIRE(result.count==1U);
    REQUIRE(umi_fs_read_text(path[1],&disk,&diskBytes)==UMI_STATUS_OK);
    const char *expectedText=strcmp(name,"save")==0?"changed Notes\n":"original\n";
    REQUIRE(diskBytes==strlen(expectedText) && memcmp(disk,expectedText,diskBytes)==0);
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
