/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_focus_activation.inc
 *
 * PURPOSE:
 *   Select the focused source document without rebuilding split editor groups.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* A notebook emits switch-page only when its selected page changes. Moving
 * between two already visible editor groups therefore needs a separate focus
 * route. Keep this in Framework so menu commands and thin clients agree about
 * the active source. This handler changes model identity only: refreshing or
 * grabbing focus here would reenter GTK's ongoing focus transition. */
static void EditorFocusEntered(GtkEventControllerFocus *controller, gpointer data)
{
    UmiGtk4EditorBinding *binding = data;
    if (binding == NULL || binding->adapter == NULL ||
        binding->adapter->shell == NULL || binding->adapter->applying_document_state ||
        !gtk_event_controller_focus_contains_focus(controller)) return;
    GtkWidget *view = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(controller));
    if (!GTK_IS_TEXT_VIEW(view)) return;
    if (gtk_widget_get_root(view) == NULL) return;
    /* The controller already describes the entering subtree. GTK can clear
     * the root focus pointer while delivering this transition, so querying
     * that pointer here would reject a valid entry before GTK commits it. */

    UmiUiWorkbench *workbench = umi_ui_application_shell_workbench(binding->adapter->shell);
    UmiUiDocumentViewSnapshot document;
    if (umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),
        binding->view_id, &document) != UMI_STATUS_OK) return;
    const UmiUiDocumentViewSnapshot *previous = g_object_get_data(
        G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view))), "umicom-editor-snapshot");
    /* A host can retain a closed page while a replacement reuses its view ID.
     * Neither that old page nor its native controller may claim the new source. */
    if (previous == NULL || strcmp(previous->document_id, document.document_id) != 0)
        return;
    UmiUiWorkbenchSnapshot current;
    if (umi_ui_workbench_snapshot(workbench, &current) != UMI_STATUS_OK ||
        strcmp(current.active_document_view, binding->view_id) == 0) return;
    (void)umi_ui_workbench_activate_document(workbench, binding->view_id);
}

static void EditorInstallFocusActivation(GtkWidget *view, UmiGtk4Adapter *adapter,
    const char *viewId)
{
    GtkEventController *focus = gtk_event_controller_focus_new();
    gtk_event_controller_set_name(focus, "umicom-source-editor-focus");
    g_signal_connect_data(focus, "enter", G_CALLBACK(EditorFocusEntered),
        editor_binding_new(adapter, viewId), editor_binding_free, 0);
    gtk_widget_add_controller(view, focus);
}
