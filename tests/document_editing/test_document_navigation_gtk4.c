/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_editing/test_document_navigation_gtk4.c
 * PURPOSE: Check real GTK editing-result delivery and the source-location form.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
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
/* Native observers can retire a form as soon as GTK publishes or presents it.
 * Retained windows make the subsequent assertions independent of finalisation. */
typedef struct FormObservation {
    UmiGtk4Adapter *adapter;
    UmiDocumentCoordinator *documents;
    Result *result;
    GtkWindow *parent;
    GtkWindow *observed;
    const char *mode;
    gulong visibleHandler;
    unsigned fired;
    UmiStatus nestedStatus;
} FormObservation;
/* Flushed phase evidence localises native presentation exceptions without
 * suppressing GTK diagnostics or changing the lifetime assertions. */
static void PresentationTrace(const char *mode, const char *phase)
{
    if (strncmp(mode, "present-", 8U) != 0) return;
    fprintf(stderr, "navigation [%s] %s\n", mode, phase);
    fflush(stderr);
}
static void FormPresented(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    FormObservation *observation = context;
    if (observation->fired || !gtk_widget_get_visible(GTK_WIDGET(object))) return;
    observation->fired = 1U;
    PresentationTrace(observation->mode, "visible observer entered");
    if (strcmp(observation->mode, "present-close") == 0)
        gtk_window_destroy(GTK_WINDOW(object));
    else
        observation->nestedStatus = UmiGtk4AdapterBindDocumentEditing(observation->adapter, NULL, NULL, NULL);
    PresentationTrace(observation->mode, "visible observer returned");
}
static void FormAdded(GListModel *model, guint position, guint removed, guint added, gpointer context)
{
    (void)removed;
    FormObservation *observation = context;
    if (observation->observed != NULL) return;
    for (guint index = 0U; index < added; ++index) {
        GtkWindow *window = g_list_model_get_item(model, position + index);
        if (window != observation->parent) {
            observation->observed = window;
            if (strcmp(observation->mode, "construction-detach") == 0) {
                observation->fired = 1U;
                observation->nestedStatus = UmiGtk4AdapterBindDocumentEditing(observation->adapter, NULL, NULL, NULL);
            } else if (strcmp(observation->mode, "construction-reenter") == 0) {
                observation->fired = 1U;
                observation->nestedStatus = UmiGtk4AdapterPromptDocumentLocation(observation->adapter);
            } else observation->visibleHandler = g_signal_connect(window, "notify::visible", G_CALLBACK(FormPresented), observation);
            return;
        }
        g_object_unref(window);
    }
}
static void CloseFormOnFeedback(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    gtk_window_destroy(GTK_WINDOW(context));
}

int main(int argc,char **argv)
{
    int failed=0;
    UmiCommandRegistry *commands=NULL;UmiUiWorkbench *workbench=NULL;
    UmiUiApplicationShell *shell=NULL;UmiDocumentStore *store=NULL;
    UmiDocumentCoordinator *documents=NULL;UmiGtk4Adapter *adapter=NULL;
    GtkApplication *application=NULL;GtkWindow *form=NULL;GtkWidget *retained=NULL;
    char viewId[UMI_UI_ID_CAPACITY];Result result={0};
    FormObservation observation = {0};
    GListModel *toplevels = NULL;
    GtkWindow *foreign = NULL, *replacement = NULL;
    GtkWidget *observedLabel = NULL;
    gulong addedHandler = 0U, feedbackHandler = 0U;
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
    if (strncmp(argv[1], "present-", 8U) == 0 || strncmp(argv[1], "construction-", 13U) == 0) {
        observation.adapter = adapter;
        observation.documents = documents;
        observation.result = &result;
        observation.parent = GTK_WINDOW(umi_gtk4_adapter_native_window(adapter));
        observation.mode = argv[1];
        toplevels = gtk_window_get_toplevels();
        addedHandler = g_signal_connect(toplevels, "items-changed", G_CALLBACK(FormAdded), &observation);
        PresentationTrace(argv[1], "initial prompt starting");
        UmiStatus status = UmiGtk4AdapterPromptDocumentLocation(adapter);
        PresentationTrace(argv[1], "initial prompt returned");
        g_signal_handler_disconnect(toplevels, addedHandler); addedHandler = 0U;
        REQUIRE(observation.fired == 1U);
        if (strcmp(argv[1], "construction-reenter") == 0) {
            REQUIRE(status == UMI_STATUS_OK && observation.nestedStatus == UMI_STATUS_BUSY);
            form = FindForm(); REQUIRE(form == observation.observed);
        } else {
            REQUIRE(status == UMI_STATUS_CANCELLED);
            REQUIRE(!UmiGtk4WindowIsOpen(observation.observed));
            if (strcmp(argv[1], "present-close") != 0) REQUIRE(observation.nestedStatus == UMI_STATUS_OK);
            PresentationTrace(argv[1], "replacement binding starting");
            REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Completed, &result) == UMI_STATUS_OK);
            PresentationTrace(argv[1], "replacement prompt starting");
            REQUIRE(UmiGtk4AdapterPromptDocumentLocation(adapter) == UMI_STATUS_OK);
            PresentationTrace(argv[1], "replacement prompt returned");
            form = FindForm(); REQUIRE(form != NULL && form != observation.observed);
        }
        REQUIRE(result.count == 0U);
        REQUIRE(umi_ui_document_view_model_find(views,viewId,&after)==UMI_STATUS_OK);
        REQUIRE(after.cursor_offset == 1U && after.selection_length == 3U);
        goto cleanup;
    }
    REQUIRE(UmiGtk4AdapterPromptDocumentLocation(adapter)==UMI_STATUS_OK);Pump();
    form=FindForm();REQUIRE(form!=NULL);
    GtkWidget *entry=Tagged(GTK_WIDGET(form),"document.location.input");
    GtkWidget *go=Tagged(GTK_WIDGET(form),"document.location.go");
    GtkWidget *cancel=Tagged(GTK_WIDGET(form),"document.location.cancel");
    REQUIRE(entry&&go&&cancel);
    if (strcmp(argv[1], "retained-destroy") == 0) {
        retained = g_object_ref(go);
        gtk_editable_set_text(GTK_EDITABLE(entry), "2:2");
        gtk_window_destroy(form);
        g_signal_emit_by_name(retained, "clicked");
        REQUIRE(result.count == 0U);
        REQUIRE(umi_ui_document_view_model_find(views,viewId,&after)==UMI_STATUS_OK);
        REQUIRE(after.cursor_offset == 1U && after.selection_length == 3U);
        REQUIRE(UmiGtk4AdapterPromptDocumentLocation(adapter) == UMI_STATUS_OK);
        replacement = FindForm(); REQUIRE(replacement != NULL && replacement != form);
        goto cleanup;
    }
    if (strcmp(argv[1], "reparented-cancel") == 0) {
        retained = g_object_ref(cancel);
        gtk_box_remove(GTK_BOX(gtk_widget_get_parent(cancel)), cancel);
        foreign = g_object_ref(GTK_WINDOW(gtk_window_new()));
        gtk_window_set_child(foreign, cancel);
        g_signal_emit_by_name(cancel, "clicked");
        REQUIRE(UmiGtk4WindowIsOpen(form) && UmiGtk4WindowIsOpen(foreign));
        REQUIRE(result.count == 0U);
        goto cleanup;
    }
    if (strcmp(argv[1], "error-feedback-close") == 0) {
        GtkWidget *label = Tagged(GTK_WIDGET(form), "document.location.message");
        REQUIRE(GTK_IS_LABEL(label));
        observedLabel = g_object_ref(label);
        feedbackHandler = g_signal_connect(label, "notify::label", G_CALLBACK(CloseFormOnFeedback), form);
        gtk_editable_set_text(GTK_EDITABLE(entry), "-1");
        g_signal_emit_by_name(go, "clicked");
        REQUIRE(!UmiGtk4WindowIsOpen(form));
        REQUIRE(result.count == 1U && result.status == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_ui_document_view_model_find(views,viewId,&after)==UMI_STATUS_OK);
        REQUIRE(after.cursor_offset == 1U && after.selection_length == 3U);
        goto cleanup;
    }
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
    PresentationTrace(argv[1], "cleanup starting");
    if (addedHandler != 0U) g_signal_handler_disconnect(toplevels, addedHandler);
    if (observation.visibleHandler != 0U) g_signal_handler_disconnect(observation.observed, observation.visibleHandler);
    if (feedbackHandler != 0U) g_signal_handler_disconnect(observedLabel, feedbackHandler);
    g_clear_object(&observedLabel);
    if (foreign != NULL) { gtk_window_destroy(foreign); g_object_unref(foreign); }
    if (replacement != NULL) { gtk_window_destroy(replacement); g_object_unref(replacement); }
    g_clear_object(&observation.observed);
    if(adapter)(void)UmiGtk4AdapterBindDocumentEditing(adapter,NULL,NULL,NULL);
    if(form){gtk_window_destroy(form);g_object_unref(form);}
    if(retained)g_object_unref(retained);
    /* Preserve the combined step; phase evidence separates queued message
     * dispatch from native adapter destruction. No cleanup is skipped. */
#if 0
    Pump();umi_gtk4_adapter_destroy(adapter);
#endif
    PresentationTrace(argv[1], "cleanup message dispatch starting");
    Pump();
    PresentationTrace(argv[1], "cleanup adapter destruction starting");
    umi_gtk4_adapter_destroy(adapter);
    PresentationTrace(argv[1], "cleanup adapter destruction returned");
    umi_document_coordinator_destroy(documents);umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);if(application)g_object_unref(application);
    PresentationTrace(argv[1], "cleanup complete");
    return failed;
}
