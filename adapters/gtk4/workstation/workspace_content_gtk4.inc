/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_content_gtk4.inc
 * PURPOSE: Preserve live provider widgets while workspace wrappers are rebuilt.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* The mount is a disposable, strongly held box. The body is the provider's
 * live view. Separating them lets old retained frames lose their body safely. */
typedef struct RetainedWorkspaceContent {
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char tool_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    GtkWidget *body;
    GtkWidget *mount;
    int invalidated;
} RetainedWorkspaceContent;

/* Detach only from the box created here, never from an application-owned
 * container. The independent body reference keeps entries and cursors alive. */
static void retained_content_unmount_entry(RetainedWorkspaceContent *entry)
{
    if (entry == NULL || entry->mount == NULL) return;
    if (entry->body != NULL && gtk_widget_get_parent(entry->body) == entry->mount)
        gtk_box_remove(GTK_BOX(entry->mount), entry->body);
    g_object_unref(entry->mount);
    entry->mount = NULL;
}

/* Release the body only at explicit refresh, identity replacement or teardown.
 * Cancel nested Framework frame actions before a separately retained old body
 * can outlive the service owner that its callbacks used. */
static void retained_content_destroy(gpointer data)
{
    RetainedWorkspaceContent *entry = data;
    if (entry == NULL) return;
    retained_content_unmount_entry(entry);
    if (entry->body != NULL) {
        umi_gtk4_ws_panel_frame_invalidate_actions(entry->body);
        umi_gtk4_ws_tab_host_invalidate_actions(entry->body);
        g_object_unref(entry->body);
    }
    g_free(entry);
}

/* IDs identify instances, not titles; renaming a tab must not erase its draft. */
static RetainedWorkspaceContent *retained_content_find(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    guint index;
    if (host->retained_content == NULL) return NULL;
    for (index = 0U; index < host->retained_content->len; ++index) {
        RetainedWorkspaceContent *entry = g_ptr_array_index(host->retained_content, index);
        if (strcmp(entry->window_id, window_id) == 0) return entry;
    }
    return NULL;
}

/* An externally destroyed detached window must not invalidate controls in a
 * body retained for later reopening. Model rebuilds already unmount all bodies. */
static void retained_content_unmount_window(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    RetainedWorkspaceContent *entry;
    if (host == NULL || window_id == NULL) return;
    entry = retained_content_find(host, window_id);
    retained_content_unmount_entry(entry);
}

/* Reusing an instance ID for another tool must never expose the previous
 * provider's private body. Hidden replacements count too. */
static int retained_content_replaced(const RetainedWorkspaceContent *entry,
                                     const UmiUiWorkspaceLayout *layout)
{
    size_t index;
    for (index = 0U; index < layout->window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[index];
        if (strcmp(window->window_id, entry->window_id) == 0)
            return strcmp(window->tool_id, entry->tool_id) != 0;
    }
    return 0;
}

/* Check the bounded cache before changing any existing widgets. Rejecting an
 * over-capacity layout must not discard a user's hidden drafts to make room. */
static UmiStatus retained_content_check_capacity(UmiGtk4WorkspaceLayoutHost *host,
                                                const UmiUiWorkspaceLayout *layout)
{
    size_t count = 0U;
    guint index;
    if (!host->retain_content) return UMI_STATUS_OK;
    for (index = 0U; index < host->retained_content->len; ++index) {
        const RetainedWorkspaceContent *entry = g_ptr_array_index(host->retained_content, index);
        if (!entry->invalidated && !retained_content_replaced(entry, layout)) ++count;
    }
    for (size_t window_index = 0U; window_index < layout->window_count; ++window_index) {
        const UmiUiWorkspaceWindow *window = &layout->windows[window_index];
        RetainedWorkspaceContent *entry;
        if (!window->visible && tool_window_edge_index(window) < 0) continue;
        entry = retained_content_find(host, window->window_id);
        if (entry == NULL || entry->invalidated || strcmp(entry->tool_id, window->tool_id) != 0)
            ++count;
    }
    return count <= UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS
        ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}

/* Remove bodies before clearing their old outer frames, including frames
 * another client still retains. Their controls remain usable in the new view. */
static void retained_content_unmount(UmiGtk4WorkspaceLayoutHost *host)
{
    guint index;
    if (host->retained_content == NULL) return;
    for (index = 0U; index < host->retained_content->len; ++index)
        retained_content_unmount_entry(g_ptr_array_index(host->retained_content, index));
}

/* Explicit invalidation is consumed only after model validation succeeds. */
static void retained_content_prune(UmiGtk4WorkspaceLayoutHost *host,
                                   const UmiUiWorkspaceLayout *layout)
{
    guint index = 0U;
    while (index < host->retained_content->len) {
        RetainedWorkspaceContent *entry = g_ptr_array_index(host->retained_content, index);
        if (entry->invalidated || retained_content_replaced(entry, layout))
            g_ptr_array_remove_index(host->retained_content, index);
        else
            ++index;
    }
    host->content_invalidated = 0;
}

/* Only opted-in hosts reuse bodies. Other applications retain the established
 * behaviour in which an explicit rebuild asks the provider for fresh content. */
static GtkWidget *retained_content_create(UmiGtk4WorkspaceLayoutHost *host,
                                         const UmiUiWorkspaceWindow *window)
{
    RetainedWorkspaceContent *entry;
    GtkWidget *body;
    if (!host->retain_content)
        return host->panel_factory != NULL
            ? host->panel_factory(window, host->panel_user_data) : NULL;
    entry = retained_content_find(host, window->window_id);
    if (entry == NULL) {
        body = host->panel_factory != NULL
            ? host->panel_factory(window, host->panel_user_data) : NULL;
        /* A factory cannot donate a widget still owned by another container.
         * Leave it untouched and let the host display its unavailable state. */
        if (body == NULL || gtk_widget_get_parent(body) != NULL) return NULL;
        entry = g_new0(RetainedWorkspaceContent, 1);
        memcpy(entry->window_id, window->window_id, strlen(window->window_id) + 1U);
        memcpy(entry->tool_id, window->tool_id, strlen(window->tool_id) + 1U);
        entry->body = g_object_ref_sink(body);
        g_ptr_array_add(host->retained_content, entry);
    }
    entry->mount = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_widget_set_hexpand(entry->body, TRUE);
    gtk_widget_set_vexpand(entry->body, TRUE);
    gtk_box_append(GTK_BOX(entry->mount), entry->body);
    return entry->mount;
}

/* Policy changes are restricted to an empty host so switching modes cannot
 * silently discard already rendered application views. */
UmiStatus umi_gtk4_workspace_layout_host_set_content_retention(
    UmiGtk4WorkspaceLayoutHost *host, int enabled)
{
    if (host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    enabled = enabled != 0;
    if (host->retain_content == enabled) return UMI_STATUS_OK;
    if (host->plan.visible_window_count != 0U || host->retained_content->len != 0U)
        return UMI_STATUS_BUSY;
    host->retain_content = enabled;
    return UMI_STATUS_OK;
}

/* Mark entries for replacement rather than destroying widgets in the middle
 * of a button callback. The next accepted rebuild performs the release. */
UmiStatus umi_gtk4_workspace_layout_host_invalidate_content(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    guint index;
    int found = window_id == NULL;
    if (host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!host->retain_content) return UMI_STATUS_INVALID_STATE;
    for (index = 0U; index < host->retained_content->len; ++index) {
        RetainedWorkspaceContent *entry = g_ptr_array_index(host->retained_content, index);
        if (window_id == NULL || strcmp(window_id, entry->window_id) == 0) {
            entry->invalidated = 1;
            host->content_invalidated = 1;
            found = 1;
        }
    }
    return found ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
