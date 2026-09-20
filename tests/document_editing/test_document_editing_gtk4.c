/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_editing/test_document_editing_gtk4.c
 * PURPOSE: Exercise the real editor binding, native clipboard and late replies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); failed=1; goto cleanup; } } while (0)
typedef struct Result { unsigned count; UmiStatus status; } Result;
static void Completed(void *context, UmiStatus status)
{ Result *r=context; r->status=status; ++r->count; }
static void Pump(void)
{ for (unsigned i=0; i<32U && g_main_context_pending(NULL); ++i) (void)g_main_context_iteration(NULL,FALSE); }
static int Await(Result *r, unsigned count)
{
    gint64 until=g_get_monotonic_time()+3000000;
    while(r->count<count && g_get_monotonic_time()<until) { Pump();g_usleep(1000); }
    return r->count>=count;
}
static int TextEquals(UmiUiDocumentViewModel *views,const char *viewId,const char *expected)
{
    char *text=NULL;size_t bytes=0U;
    UmiStatus status=UmiUiDocumentViewModelCopyText(views,viewId,&text,&bytes);
    int equal=status==UMI_STATUS_OK && bytes==strlen(expected) && memcmp(text,expected,bytes)==0;
    UmiUiDocumentViewModelFreeText(text);return equal;
}
static GtkWidget *FindEditor(GtkWidget *root)
{
    if(GTK_IS_TEXT_VIEW(root) && gtk_widget_has_css_class(root,"umicom-editor"))return root;
    for(GtkWidget *child=gtk_widget_get_first_child(root);child;child=gtk_widget_get_next_sibling(child)) {
        GtkWidget *found=FindEditor(child);if(found)return found;
    }
    return NULL;
}
static void DetachOnClipboardChange(GdkClipboard *clipboard,gpointer data)
{ (void)clipboard;(void)UmiGtk4AdapterBindDocumentEditing(data,NULL,NULL,NULL); }
int main(int argc,char **argv)
{
    UmiCommandRegistry *commands=NULL;
    UmiUiWorkbench *workbench=NULL;
    UmiUiApplicationShell *shell=NULL;
    UmiDocumentStore *store=NULL;
    UmiDocumentCoordinator *documents=NULL;
    UmiGtk4Adapter *adapter=NULL;
    GtkApplication *application=NULL;
    GdkClipboard *clipboard=NULL;
    gulong changed=0UL;
    Result result={0};
    char first[UMI_UI_ID_CAPACITY],second[UMI_UI_ID_CAPACITY];
    int failed=0;
    if(argc!=2)return 2;
    if(!gtk_init_check())return 77;
    application=gtk_application_new("org.umicom.editing.test",G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(application),NULL,NULL));
    REQUIRE(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("umicom.editing.test",commands,&workbench)==UMI_STATUS_OK);
    REQUIRE(umi_ui_application_shell_create("org.umicom.editing.test","Umicom Notes",workbench,&shell)==UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store)==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_create(store,workbench,NULL,&documents)==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_new(documents,"notes.c",first,sizeof first)==UMI_STATUS_OK);
    UmiUiDocumentViewModel *views=umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    REQUIRE(umi_ui_document_view_model_find(views,first,&view)==UMI_STATUS_OK);
    view.dirty=1;view.cursor_offset=0U;view.selection_length=6U;
    REQUIRE(UmiUiDocumentViewModelUpsertText(views,&view,"Umicom Notes",12U)==UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot target;
    REQUIRE(umi_document_coordinator_active_snapshot(documents,&target)==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCoordinatorSyncDocument(documents,target.document_id)==UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_create(application,&adapter)==UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_prepare(adapter,shell)==UMI_STATUS_OK);
    REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,documents,Completed,&result)==UMI_STATUS_OK);
    GtkWidget *window=umi_gtk4_adapter_native_window(adapter);
    REQUIRE(GTK_IS_WINDOW(window));
    clipboard=gtk_widget_get_clipboard(window);
    REQUIRE(clipboard!=NULL);

    if(strcmp(argv[1],"source-signals")==0) {
        GtkWidget *editor=FindEditor(window);REQUIRE(editor!=NULL);
        GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
        GtkTextIter end;gtk_text_buffer_get_end_iter(buffer,&end);
        gtk_text_buffer_begin_user_action(buffer);
        gtk_text_buffer_insert(buffer,&end," project",8);
        gtk_text_buffer_end_user_action(buffer);
        REQUIRE(TextEquals(views,first,"Umicom Notes project"));
        g_signal_emit_by_name(buffer,"undo");
        REQUIRE(TextEquals(views,first,"Umicom Notes"));
        g_signal_emit_by_name(buffer,"redo");
        REQUIRE(TextEquals(views,first,"Umicom Notes project"));
        goto cleanup;
    }
    if(strcmp(argv[1],"readonly")==0) {
        REQUIRE(umi_ui_document_view_model_find(views,first,&view)==UMI_STATUS_OK);
        view.read_only=1;REQUIRE(umi_ui_document_view_model_upsert(views,&view)==UMI_STATUS_OK);
        REQUIRE(UmiGtk4AdapterDocumentCommandEnabled(adapter,"edit.copy"));
        REQUIRE(!UmiGtk4AdapterDocumentCommandEnabled(adapter,"edit.cut"));
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,"edit.cut")==UMI_STATUS_PERMISSION_DENIED);
        REQUIRE(TextEquals(views,first,"Umicom Notes"));goto cleanup;
    }
    if(strcmp(argv[1],"cut-detach")==0) {
        changed=g_signal_connect(clipboard,"changed",G_CALLBACK(DetachOnClipboardChange),adapter);
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,"edit.cut")==UMI_STATUS_CANCELLED);
        REQUIRE(TextEquals(views,first,"Umicom Notes"));goto cleanup;
    }
    if(strcmp(argv[1],"clipboard")==0) {
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,"edit.copy")==UMI_STATUS_OK);
        REQUIRE(TextEquals(views,first,"Umicom Notes"));
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,"edit.cut")==UMI_STATUS_OK);
        REQUIRE(TextEquals(views,first," Notes"));
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,"edit.undo")==UMI_STATUS_OK);
        REQUIRE(TextEquals(views,first,"Umicom Notes"));
        REQUIRE(umi_ui_document_view_model_find(views,first,&view)==UMI_STATUS_OK);
        view.cursor_offset=0U;view.selection_length=6U;
        REQUIRE(umi_ui_document_view_model_upsert(views,&view)==UMI_STATUS_OK);
    }
    gdk_clipboard_set_text(clipboard,"Framework");
    unsigned expected=result.count+1U;
    REQUIRE(UmiGtk4AdapterDocumentCommand(adapter,"edit.paste")==UMI_STATUS_OK);
    REQUIRE(!UmiGtk4AdapterDocumentCommandEnabled(adapter,"edit.paste"));
    UmiStatus expectedStatus=UMI_STATUS_OK;
    if(strcmp(argv[1],"stale-draft")==0) {
        REQUIRE(umi_ui_document_view_model_find(views,first,&view)==UMI_STATUS_OK);
        REQUIRE(UmiUiDocumentViewModelUpsertText(views,&view,"Newer notes",11U)==UMI_STATUS_OK);
        expectedStatus=UMI_STATUS_INVALID_STATE;
    } else if(strcmp(argv[1],"other-tab")==0) {
        REQUIRE(umi_document_coordinator_new(documents,"review.txt",second,sizeof second)==UMI_STATUS_OK);
        REQUIRE(umi_ui_document_view_model_find(views,second,&view)==UMI_STATUS_OK);
        view.dirty=1;REQUIRE(UmiUiDocumentViewModelUpsertText(views,&view,"Keep this draft",15U)==UMI_STATUS_OK);
    } else if(strcmp(argv[1],"closed")==0) {
        REQUIRE(UmiDocumentCoordinatorClose(documents,target.document_id,1)==UMI_STATUS_OK);
        expectedStatus=UMI_STATUS_NOT_FOUND;
    } else if(strcmp(argv[1],"unbind")==0) {
        REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter,NULL,NULL,NULL)==UMI_STATUS_OK);
        for(unsigned n=0;n<100U;++n){Pump();g_usleep(1000);}
        REQUIRE(result.count==expected-1U && TextEquals(views,first,"Umicom Notes"));goto cleanup;
    } else REQUIRE(strcmp(argv[1],"clipboard")==0);
    REQUIRE(Await(&result,expected));REQUIRE(result.status==expectedStatus);
    if(strcmp(argv[1],"closed")!=0)
        REQUIRE(TextEquals(views,first,expectedStatus==UMI_STATUS_OK?"Framework Notes":"Newer notes"));
    if(strcmp(argv[1],"other-tab")==0) {
        UmiDocumentWorkingCopySnapshot active;
        REQUIRE(umi_document_coordinator_active_snapshot(documents,&active)==UMI_STATUS_OK);
        REQUIRE(strcmp(active.view_id,second)==0 && TextEquals(views,second,"Keep this draft"));
    }
cleanup:
    if(changed && clipboard)g_signal_handler_disconnect(clipboard,changed);
    if(adapter)(void)UmiGtk4AdapterBindDocumentEditing(adapter,NULL,NULL,NULL);
    /* Drain cancelled clipboard callbacks before destroying their owners. */
    for(unsigned n=0;n<100U;++n){Pump();g_usleep(1000);}
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if(application)g_object_unref(application);
    return failed;
}
