/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_focus_gtk4.inc
 *
 * PURPOSE:
 *   Select the existing panel while retaining host and widget authority across synchronous GTK observers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* A callback can retire this host, but focus_active keeps its allocation alive
 * until the caller releases all borrowed state. Rebuild and maximise requests
 * are rejected during this short synchronous transaction. */
static gboolean WorkspaceFocusOwnerCurrent(
    UmiGtk4WorkspaceLayoutHost *host, uint64_t revision)
{
    return !host->destroy_requested && !host->destroying && host->revision == revision;
}

static GtkWidget *WorkspaceFocusFrame(UmiGtk4WorkspaceLayoutHost *host,
    const char *id, GtkWindow **floating)
{
    *floating = NULL;
    const UmiUiWorkspaceWindow *window = umi_ui_workspace_layout_find_window(&host->layout, id);
    if (window == NULL || !window->visible) return NULL;
    if (window->floating) {
        for (guint i = 0U; i < host->floating_windows->len; ++i) {
            FloatingWindowEntry *entry = g_ptr_array_index(host->floating_windows, i);
            if (!entry->destroyed && strcmp(entry->window_id, id) == 0) {
                *floating = entry->window;
                return entry->frame;
            }
        }
        return NULL;
    }
    return umi_ws_maximize_mode_matches(&host->maximise_mode, id)
        ? host->maximise_frame : find_live_window_frame(host->root, id);
}

/* Panel selection is valid before an application presents its native window.
 * Check the panel and every content ancestor, but leave root visibility to the
 * application. A closed native window is rejected separately by identity. */
static gboolean WorkspaceFocusContentVisible(GtkWidget *widget)
{
    for (GtkWidget *current = widget; current != NULL && !GTK_IS_ROOT(current);
         current = gtk_widget_get_parent(current)) {
        if (!gtk_widget_get_visible(current)) return FALSE;
    }
    return TRUE;
}

/* Selection is proven by actual frame and native-root identity. A retained
 * widget alone does not prove it still belongs to this workspace. */
static gboolean WorkspaceFocusTargetCurrent(UmiGtk4WorkspaceLayoutHost *host,
    uint64_t revision, const char *id, GtkWidget *frame, GtkRoot *native,
    GtkNotebook *notebook)
{
    if (!WorkspaceFocusOwnerCurrent(host, revision)) return FALSE;
    GtkWindow *floating = NULL;
    if (WorkspaceFocusFrame(host, id, &floating) != frame ||
/* Root presentation is independent of panel visibility; keep callback ownership checks. The former implementation is retained for owner review. */
#if 0
        gtk_widget_get_root(frame) != native || !gtk_widget_is_visible(frame) ||
#endif
        gtk_widget_get_root(frame) != native || !WorkspaceFocusContentVisible(frame) ||
        !gtk_widget_is_sensitive(frame)) return FALSE;
    if (GTK_IS_WINDOW(native) && !UmiGtk4WindowIsOpen(GTK_WINDOW(native))) return FALSE;
    if (notebook != NULL) {
        int page = gtk_notebook_get_current_page(notebook);
        if (gtk_widget_get_ancestor(frame, GTK_TYPE_NOTEBOOK) != GTK_WIDGET(notebook) ||
            page < 0 || gtk_notebook_get_nth_page(notebook, page) != frame) return FALSE;
    }
    return TRUE;
}


/* Count notifications rather than comparing only the final pointer: an observer
 * may redirect focus back to the original outside editor during the same call. */
static void WorkspaceFocusChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    ++*(unsigned *)data;
}

UmiStatus umi_gtk4_workspace_layout_host_focus_window(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    if (host == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (host->destroy_requested || host->destroying) return UMI_STATUS_CANCELLED;
    if (host->focus_active) return UMI_STATUS_BUSY;
    const UmiUiWorkspaceWindow *window =
        umi_ui_workspace_layout_find_window(&host->layout, window_id);
    if (window == NULL || !window->visible) return UMI_STATUS_NOT_FOUND;
    char *id = g_strdup(window_id);
    uint64_t revision = host->revision;
    GtkWidget *frame = NULL, *before_focus = NULL;
    GtkNotebook *notebook = NULL;
    GtkRoot *native = NULL;
    GtkWindow *floating = NULL;
    UmiStatus status = UMI_STATUS_CANCELLED;
    unsigned focus_changes = 0U;
    gulong focus_handler = 0U;
    host->focus_active = 1;
    if (host->maximise_mode.active && !umi_ws_maximize_mode_matches(&host->maximise_mode, id))
        restore_maximised_presentation(host, 0);
    if (!WorkspaceFocusOwnerCurrent(host, revision)) goto done;
    frame = WorkspaceFocusFrame(host, id, &floating);
    if (frame == NULL) { status = UMI_STATUS_NOT_FOUND; goto done; }
    g_object_ref(frame);
/* Select visible content in an unpresented root without presenting or resurrecting the owner. The former implementation is retained for owner review. */
#if 0
    if (!gtk_widget_get_visible(frame) || (!floating && !gtk_widget_is_visible(frame)) ||
        !gtk_widget_is_sensitive(frame)) {
#endif
    if (!WorkspaceFocusContentVisible(frame) || !gtk_widget_is_sensitive(frame)) {
        status = UMI_STATUS_UNAVAILABLE; goto done;
    }
    native = gtk_widget_get_root(frame);
    if (native != NULL) g_object_ref(native);
    if (GTK_IS_WINDOW(native) && !UmiGtk4WindowIsOpen(GTK_WINDOW(native))) goto done;
    if (floating != NULL) {
        g_object_ref(floating);
/* Separate visibility notification from activation so closed native surfaces are not focused. The former implementation is retained for owner review. */
#if 0
        gtk_window_present(floating);
#endif
        /* Showing may synchronously close the window through an observer.
         * Finish that notification and recheck ownership before GTK starts its
         * native activation sequence. */
        gtk_widget_set_visible(GTK_WIDGET(floating), TRUE);
        if (WorkspaceFocusTargetCurrent(host, revision, id, frame, native, NULL))
            gtk_window_present(floating);
        g_object_unref(floating);
        if (!WorkspaceFocusTargetCurrent(host, revision, id, frame, native, NULL)) goto done;
    }
    GtkWidget *ancestor = gtk_widget_get_ancestor(frame, GTK_TYPE_NOTEBOOK);
    if (ancestor != NULL) {
        notebook = g_object_ref(GTK_NOTEBOOK(ancestor));
        int page = gtk_notebook_page_num(notebook, frame);
        if (page < 0) goto done;
        gtk_notebook_set_current_page(notebook, page);
        if (!WorkspaceFocusTargetCurrent(host, revision, id, frame, native, notebook)) goto done;
    }
    /* Canvas elevation is presentation only. Never continue through a callback
     * that removed the target, retired the host or replaced its source revision. */
    for (guint i = 0U; i < host->canvas_entries->len; ++i) {
        CanvasEntry *entry = g_ptr_array_index(host->canvas_entries, i);
        if (strcmp(entry->window_id, id) == 0 && host->canvas_layer != NULL) {
            GtkWidget *last = gtk_widget_get_last_child(host->canvas_layer);
            if (last != entry->panel)
                gtk_widget_insert_after(entry->panel, host->canvas_layer, last);
            break;
        }
    }
    if (!WorkspaceFocusTargetCurrent(host, revision, id, frame, native, notebook)) goto done;
    /* An unmounted host may select its notebook page, but it has no native
     * root to receive keyboard focus. Defer traversal until it is mounted. */
    if (native == NULL) { status = UMI_STATUS_OK; goto done; }
    before_focus = native != NULL ? gtk_root_get_focus(native) : NULL;
    if (before_focus != NULL) g_object_ref(before_focus);
    if (before_focus != NULL &&
        (before_focus == frame || gtk_widget_is_ancestor(before_focus, frame)) &&
        gtk_widget_is_visible(before_focus) && gtk_widget_is_sensitive(before_focus)) {
        status = UMI_STATUS_OK;
        goto done;
    }
    if (GTK_IS_WINDOW(native))
        focus_handler = g_signal_connect(native, "notify::focus-widget",
            G_CALLBACK(WorkspaceFocusChanged), &focus_changes);
    gboolean focused = gtk_widget_child_focus(frame, GTK_DIR_TAB_FORWARD);
    if (!WorkspaceFocusTargetCurrent(host, revision, id, frame, native, notebook)) goto done;
    GtkWidget *after_focus = native != NULL ? gtk_root_get_focus(native) : NULL;
    if (focus_changes != 0U && (after_focus == NULL ||
        (after_focus != frame && !gtk_widget_is_ancestor(after_focus, frame)))) goto done;
    if (!focused) {
        gtk_widget_set_focusable(frame, TRUE);
        if (!WorkspaceFocusTargetCurrent(host, revision, id, frame, native, notebook)) goto done;
        (void)gtk_widget_grab_focus(frame);
    }
    if (!WorkspaceFocusTargetCurrent(host, revision, id, frame, native, notebook)) goto done;
    after_focus = native != NULL ? gtk_root_get_focus(native) : NULL;
    if (focus_changes != 0U && (after_focus == NULL ||
        (after_focus != frame && !gtk_widget_is_ancestor(after_focus, frame)))) goto done;
    status = UMI_STATUS_OK;
done:
    if (focus_handler != 0U) g_signal_handler_disconnect(native, focus_handler);
    g_clear_object(&before_focus);
    g_clear_object(&notebook);
    g_clear_object(&native);
    g_clear_object(&frame);
    g_free(id);
    /* Destruction remains logically immediate for callers. Final release is
     * delayed only until this synchronous operation stops using the host. */
    if (host->destroy_requested) status = UMI_STATUS_CANCELLED;
    host->focus_active = 0;
    if (host->destroy_requested) umi_gtk4_workspace_layout_host_destroy(host);
    return status;
}
