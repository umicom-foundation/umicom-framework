/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_layout_host_gtk4.c
 *
 * PURPOSE:
 *   Render a complete portable workspace layout as GTK4 paned regions, tab
 *   groups, floating windows and reusable Framework panel frames.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/gtk4/workstation/panel_frame.h"
#include "umicom/ui/gtk4/workstation/tab_host.h"
#include "umicom/ui/gtk4/automation.h"

typedef struct StackEntry {
    char stack_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char placement_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    GtkWidget *notebook;
    size_t page_count;
} StackEntry;

/* Strongly owned detached-window state remains valid even after the native
 * window has emitted its destroy signal. Rebuilds can therefore clean up
 * without following stale widget pointers. */
typedef struct FloatingWindowEntry {
    UmiGtk4WorkspaceLayoutHost *host;
    GtkWindow *window;
    gulong close_handler_id;
    gulong destroy_handler_id;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    int closable;
    int pinned;
    int destroyed;
} FloatingWindowEntry;

/* Deferred model action used by native close requests. */
typedef struct PendingHostAction {
    UmiGtk4WorkspaceLayoutHost *host;
    UmiWsPanelAction action;
    char panel_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
} PendingHostAction;

struct UmiGtk4WorkspaceLayoutHost {
    GtkWidget *root;
    UmiUiWorkspaceLayout layout;
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory;
    UmiGtk4WorkspaceLayoutPanelActionHandler action_handler;
    void *user_data;
    GtkWindow *transient_parent;
    GPtrArray *floating_windows;
    guint pending_action_id;
};

/* Dispatch a native-window request only after its current signal has returned,
 * allowing the model owner to rebuild or destroy GTK content safely. */
static gboolean dispatch_host_action_from_idle(gpointer user_data)
{
    PendingHostAction *pending = (PendingHostAction *)user_data;

    if (pending != NULL && pending->host != NULL) {
        pending->host->pending_action_id = 0U;
        if (pending->host->action_handler != NULL) {
            pending->host->action_handler(
                pending->panel_id,
                pending->action,
                pending->host->user_data);
        }
    }
    return G_SOURCE_REMOVE;
}

/* Record that a detached native window has completed destruction while
 * retaining its entry until the next model rebuild releases the strong ref. */
static void on_floating_window_destroy(
    GtkWidget *widget,
    gpointer user_data)
{
    FloatingWindowEntry *entry = (FloatingWindowEntry *)user_data;

    (void)widget;
    if (entry != NULL) entry->destroyed = 1;
}

/* A native title-bar close is a model command, not an unmanaged widget
 * deletion. Locked layouts reject accidental removal; editable layouts route
 * the close through the same portable action used by panel and tab controls. */
static gboolean on_floating_window_close_request(
    GtkWindow *window,
    gpointer user_data)
{
    FloatingWindowEntry *entry = (FloatingWindowEntry *)user_data;
    PendingHostAction *pending;

    (void)window;
    if (entry == NULL || entry->host == NULL ||
        entry->host->layout.locked || !entry->closable ||
        entry->pinned || entry->host->pending_action_id != 0U) {
        return TRUE;
    }
    pending = g_new0(PendingHostAction, 1);
    if (pending == NULL) return TRUE;
    pending->host = entry->host;
    pending->action = UMI_WS_PANEL_ACTION_CLOSE;
    (void)snprintf(
        pending->panel_id,
        sizeof(pending->panel_id),
        "%s",
        entry->window_id);
    entry->host->pending_action_id = g_idle_add_full(
        G_PRIORITY_DEFAULT_IDLE,
        dispatch_host_action_from_idle,
        pending,
        g_free);
    if (entry->host->pending_action_id == 0U) {
        g_free(pending);
    }
    return TRUE;
}

/* Release one detached-window entry in reverse ownership order. */
static void floating_window_entry_destroy(gpointer data)
{
    FloatingWindowEntry *entry = (FloatingWindowEntry *)data;

    if (entry == NULL) return;
    if (entry->window != NULL) {
        if (!entry->destroyed) {
            if (entry->close_handler_id != 0U) {
                g_signal_handler_disconnect(
                    entry->window, entry->close_handler_id);
            }
            if (entry->destroy_handler_id != 0U) {
                g_signal_handler_disconnect(
                    entry->window, entry->destroy_handler_id);
            }
            gtk_window_destroy(entry->window);
        }
        g_object_unref(entry->window);
        entry->window = NULL;
    }
    g_free(entry);
}

/* Forward semantic panel actions to the owning workstation. */
static void on_panel_action(
    const char *panel_id,
    UmiWsPanelAction action,
    void *user_data)
{
    UmiGtk4WorkspaceLayoutHost *host =
        (UmiGtk4WorkspaceLayoutHost *)user_data;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host != NULL && host->action_handler != NULL)
        host->action_handler(panel_id, action, host->user_data);
}

/* A docked tab close button uses the same semantic action as a floating panel
 * header, keeping GTK state subordinate to the portable workspace model. */
static void on_tab_close(const char *tab_id, void *user_data)
{
    on_panel_action(
        tab_id, UMI_WS_PANEL_ACTION_CLOSE, user_data);
}

/* Remove every current docked child without destroying the stable host root. */
static void clear_root(GtkWidget *root)
{
    GtkWidget *child;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL) return;
    child = gtk_widget_get_first_child(root);
    /* Continue until every requested item has been processed. */
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_box_remove(GTK_BOX(root), child);
        child = next;
    }
}

/* Close detached native windows before the model they represent is replaced. */
static void clear_floating_windows(UmiGtk4WorkspaceLayoutHost *host)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || host->floating_windows == NULL) return;
    g_ptr_array_set_size(host->floating_windows, 0U);
}

/* Return the visible stack key, falling back to the group identifier used by
 * older layouts. */
static const char *window_stack_id(const UmiUiWorkspaceWindow *window)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return "";
    /* Apply this branch only when its contract condition is satisfied. */
    if (window->stack_id[0] != '\0') return window->stack_id;
    /* Apply this branch only when its contract condition is satisfied. */
    if (window->placement_id[0] != '\0') return window->placement_id;
    return window->group_id;
}

/* Recognise the semantic regions rendered directly by this adapter. */
static bool is_known_placement(const char *placement_id)
{
    if (placement_id == NULL) return false;
    return strcmp(placement_id, "left") == 0 ||
           strcmp(placement_id, "centre") == 0 ||
           strcmp(placement_id, "right") == 0 ||
           strcmp(placement_id, "top") == 0 ||
           strcmp(placement_id, "bottom") == 0;
}

/* Unknown custom placements fall back to centre so no visible panel is lost. */
static bool window_matches_placement(
    const UmiUiWorkspaceWindow *window,
    const char *placement_id)
{
    if (window == NULL || placement_id == NULL) return false;
    if (strcmp(window->placement_id, placement_id) == 0) return true;
    return strcmp(placement_id, "centre") == 0 &&
           !is_known_placement(window->placement_id);
}

/* Release a widget that was created but never adopted by a GTK parent. */
static void release_unparented_widget(GtkWidget *widget)
{
    if (widget == NULL) return;
    g_object_ref_sink(widget);
    g_object_unref(widget);
}

/* Count visible detached windows so the main canvas can explain an otherwise
 * empty host without manufacturing a permanent placeholder panel. */
static size_t visible_floating_window_count(
    const UmiUiWorkspaceLayout *layout)
{
    size_t count = 0U;
    size_t index;

    if (layout == NULL) return 0U;
    for (index = 0U; index < layout->window_count; ++index) {
        if (layout->windows[index].visible &&
            layout->windows[index].floating) {
            count += 1U;
        }
    }
    return count;
}

/* Create a calm full-canvas empty state only when the entire host has no
 * docked content. Individual empty regions remain absent and consume no room. */
static GtkWidget *create_workspace_empty_state(
    const char *title,
    const char *message)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *heading = gtk_label_new(title != NULL ? title : "");
    GtkWidget *body = gtk_label_new(message != NULL ? message : "");

    if (box == NULL || heading == NULL || body == NULL) return box;
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_widget_add_css_class(box, "umicom-workspace-empty-state");
    gtk_widget_add_css_class(heading, "title-3");
    gtk_widget_add_css_class(body, "dim-label");
    gtk_label_set_wrap(GTK_LABEL(body), TRUE);
    gtk_label_set_justify(GTK_LABEL(body), GTK_JUSTIFY_CENTER);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), body);
    return box;
}

/* Join two populated regions and return the populated child unchanged when the
 * opposite region is empty. This prevents blank panes and unusable splitters. */
static GtkWidget *join_regions(
    GtkOrientation orientation,
    GtkWidget *start,
    GtkWidget *end,
    int position,
    bool resize_start,
    bool resize_end)
{
    GtkWidget *paned;

    if (start == NULL) return end;
    if (end == NULL) return start;
    paned = gtk_paned_new(orientation);
    if (paned == NULL) {
        release_unparented_widget(start);
        release_unparented_widget(end);
        return NULL;
    }
    gtk_paned_set_start_child(GTK_PANED(paned), start);
    gtk_paned_set_end_child(GTK_PANED(paned), end);
    gtk_paned_set_position(GTK_PANED(paned), position);
    gtk_paned_set_resize_start_child(GTK_PANED(paned), resize_start);
    gtk_paned_set_resize_end_child(GTK_PANED(paned), resize_end);
    gtk_paned_set_shrink_start_child(GTK_PANED(paned), TRUE);
    gtk_paned_set_shrink_end_child(GTK_PANED(paned), TRUE);
    gtk_widget_set_hexpand(paned, TRUE);
    gtk_widget_set_vexpand(paned, TRUE);
    return paned;
}

/* Find one stack entry by stable identifier. */
static StackEntry *find_stack(
    StackEntry *entries,
    size_t count,
    const char *stack_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entries == NULL || stack_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(entries[index].stack_id, stack_id) == 0)
            return &entries[index];
    }
    return NULL;
}

/* Create content through the application factory or a truthful placeholder. */
static GtkWidget *create_content(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiUiWorkspaceWindow *window)
{
    GtkWidget *content = host->panel_factory != NULL
        ? host->panel_factory(window, host->user_data)
        : NULL;

    /* Apply this branch only when its contract condition is satisfied. */
    if (content == NULL) {
        char text[UMI_UI_TEXT_CAPACITY + 32U];

        (void)snprintf(
            text, sizeof(text), "%s\nThis panel has no active provider.",
            window->title);
        content = gtk_label_new(text);
        gtk_label_set_wrap(GTK_LABEL(content), TRUE);
        gtk_widget_add_css_class(content, "dim-label");
        gtk_widget_set_margin_top(content, 12);
        gtk_widget_set_margin_bottom(content, 12);
        gtk_widget_set_margin_start(content, 12);
        gtk_widget_set_margin_end(content, 12);
    }
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_vexpand(content, TRUE);
    return content;
}

/* Wrap one panel with Framework chrome. Normal mode hides placement metadata
 * and condenses geometry actions into one overflow menu; Edit Layout mode
 * reveals the complete direct-control set. */
static GtkWidget *create_panel(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiUiWorkspaceWindow *window)
{
    UmiWsPanelChrome chrome;
    GtkWidget *content;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || window == NULL) return NULL;
    content = create_content(host, window);
    (void)umi_ws_panel_chrome_init(&chrome, window->title);
    (void)umi_ws_panel_chrome_set_identity(
        &chrome, window->window_id, window->placement_id);
    (void)umi_ws_panel_chrome_set_context(
        &chrome, window->context_group_id, window->context_group_id);
    (void)umi_ws_panel_chrome_set_badge(
        &chrome,
        host->layout.locked ? "" : window->placement_id);
    chrome.show_close = window->floating &&
        window->closable && !window->pinned;
    chrome.show_pin = true;
    chrome.show_menu = host->layout.locked;
    chrome.show_context = true;
    chrome.show_move = true;
    chrome.show_float = true;
    chrome.show_maximise = true;
    chrome.show_settings = true;
    chrome.compact = true;
    chrome.locked = host->layout.locked;
    return umi_gtk4_ws_panel_frame_create(
        &chrome, content, on_panel_action, host);
}

/* Create one notebook for every distinct tab stack in a dock region. Empty
 * regions return NULL so they do not consume permanent workspace space. */
static GtkWidget *build_stack(
    UmiGtk4WorkspaceLayoutHost *host,
    const char *placement_id)
{
    StackEntry entries[UMI_UI_WORKSPACE_MAX_WINDOWS];
    size_t entry_count = 0U;
    size_t visible_stack_count = 0U;
    size_t index;
    GtkWidget *container = NULL;

    (void)memset(entries, 0, sizeof(entries));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < host->layout.window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &host->layout.windows[index];
        StackEntry *entry;
        GtkWidget *frame;
        const char *stack_id;

        if (!window->visible || window->floating ||
            !window_matches_placement(window, placement_id)) {
            continue;
        }
        stack_id = window_stack_id(window);
        entry = find_stack(entries, entry_count, stack_id);
        if (entry == NULL) {
            if (entry_count >= UMI_UI_WORKSPACE_MAX_WINDOWS) continue;
            entry = &entries[entry_count++];
            (void)snprintf(
                entry->stack_id, sizeof(entry->stack_id), "%s", stack_id);
            (void)snprintf(
                entry->placement_id, sizeof(entry->placement_id), "%s",
                placement_id);
            entry->notebook = umi_gtk4_ws_tab_host_create(NULL);
            if (entry->notebook == NULL) continue;
            gtk_widget_set_hexpand(entry->notebook, TRUE);
            gtk_widget_set_vexpand(entry->notebook, TRUE);
        }
        if (entry->notebook == NULL) continue;
        frame = create_panel(host, window);
        if (frame != NULL) {
            UmiStatus status = umi_gtk4_ws_tab_host_append_managed(
                entry->notebook,
                window->window_id,
                window->title,
                frame,
                !host->layout.locked &&
                    window->closable && !window->pinned,
                on_tab_close,
                host);
            if (status == UMI_STATUS_OK) {
                entry->page_count += 1U;
            } else {
                release_unparented_widget(frame);
            }
        }
    }

    /* Adopt only notebooks that contain a visible page. */
    for (index = 0U; index < entry_count; ++index) {
        if (entries[index].notebook == NULL) continue;
        if (entries[index].page_count == 0U) {
            release_unparented_widget(entries[index].notebook);
            continue;
        }
        if (container == NULL) {
            container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
            if (container == NULL) {
                release_unparented_widget(entries[index].notebook);
                continue;
            }
            gtk_widget_set_hexpand(container, TRUE);
            gtk_widget_set_vexpand(container, TRUE);
        }
        gtk_box_append(GTK_BOX(container), entries[index].notebook);
        visible_stack_count += 1U;
    }
    return visible_stack_count > 0U ? container : NULL;
}

/* Create all detached windows after the dock tree is available. */
static void build_floating_windows(UmiGtk4WorkspaceLayoutHost *host)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < host->layout.window_count; ++index) {
        const UmiUiWorkspaceWindow *model = &host->layout.windows[index];
        FloatingWindowEntry *entry;
        GtkWindow *window;
        GtkWidget *frame;
        int width;
        int height;

        if (!model->visible || !model->floating) continue;
        window = GTK_WINDOW(gtk_window_new());
        frame = create_panel(host, model);
        if (window == NULL || frame == NULL) {
            release_unparented_widget(frame);
            if (window != NULL) gtk_window_destroy(window);
            continue;
        }
        entry = g_new0(FloatingWindowEntry, 1);
        if (entry == NULL) {
            release_unparented_widget(frame);
            gtk_window_destroy(window);
            continue;
        }

        entry->host = host;
        entry->window = window;
        entry->closable = model->closable ? 1 : 0;
        entry->pinned = model->pinned ? 1 : 0;
        (void)snprintf(
            entry->window_id, sizeof(entry->window_id), "%s",
            model->window_id);
        g_object_ref_sink(window);
        entry->close_handler_id = g_signal_connect(
            window,
            "close-request",
            G_CALLBACK(on_floating_window_close_request),
            entry);
        entry->destroy_handler_id = g_signal_connect(
            window,
            "destroy",
            G_CALLBACK(on_floating_window_destroy),
            entry);

        gtk_window_set_title(window, model->title);
        width = model->width > 1.0
            ? (int)model->width
            : (int)(model->width * 1440.0);
        height = model->height > 1.0
            ? (int)model->height
            : (int)(model->height * 900.0);
        if (width < 320) width = 320;
        if (height < 220) height = 220;
        gtk_window_set_default_size(window, width, height);
        gtk_window_set_resizable(window, model->resizable);
        if (host->transient_parent != NULL)
            gtk_window_set_transient_for(window, host->transient_parent);
        gtk_window_set_child(window, frame);
        g_ptr_array_add(host->floating_windows, entry);
        gtk_window_present(window);
    }
}

/* Create the stable host widget and retain model callbacks. */
UmiStatus umi_gtk4_workspace_layout_host_create(
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    UmiGtk4WorkspaceLayoutPanelActionHandler action_handler,
    void *user_data,
    UmiGtk4WorkspaceLayoutHost **out_host)
{
    UmiGtk4WorkspaceLayoutHost *host;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_host = NULL;
    host = (UmiGtk4WorkspaceLayoutHost *)calloc(1U, sizeof(*host));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    host->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    host->floating_windows = g_ptr_array_new_with_free_func(
        floating_window_entry_destroy);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host->root == NULL || host->floating_windows == NULL) {
        umi_gtk4_workspace_layout_host_destroy(host);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    host->panel_factory = panel_factory;
    host->action_handler = action_handler;
    host->user_data = user_data;
    gtk_widget_set_hexpand(host->root, TRUE);
    gtk_widget_set_vexpand(host->root, TRUE);
    gtk_widget_add_css_class(host->root, "umicom-workspace-layout-host");
    (void)umi_gtk4_automation_set_id(
        host->root, "workstation.workspace-layout");
    *out_host = host;
    return UMI_STATUS_OK;
}

/* Close detached windows and release host-owned containers. */
void umi_gtk4_workspace_layout_host_destroy(
    UmiGtk4WorkspaceLayoutHost *host)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL) return;
    if (host->pending_action_id != 0U) {
        (void)g_source_remove(host->pending_action_id);
        host->pending_action_id = 0U;
    }
    clear_floating_windows(host);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host->floating_windows != NULL)
        g_ptr_array_free(host->floating_windows, TRUE);
    host->floating_windows = NULL;
    host->root = NULL;
    free(host);
}

/* Set the optional transient parent for future floating panel windows. */
void umi_gtk4_workspace_layout_host_set_transient_parent(
    UmiGtk4WorkspaceLayoutHost *host,
    GtkWindow *parent)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host != NULL) host->transient_parent = parent;
}

/* Render semantic regions with nested paned containers. */
UmiStatus umi_gtk4_workspace_layout_host_rebuild(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiUiWorkspaceLayout *layout)
{
    GtkWidget *left;
    GtkWidget *centre;
    GtkWidget *right;
    GtkWidget *top;
    GtkWidget *bottom;
    GtkWidget *centre_right;
    GtkWidget *middle;
    GtkWidget *main_bottom;
    GtkWidget *workspace;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || layout == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (layout->window_count > UMI_UI_WORKSPACE_MAX_WINDOWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    host->layout = *layout;
    clear_floating_windows(host);
    clear_root(host->root);

    left = build_stack(host, "left");
    centre = build_stack(host, "centre");
    right = build_stack(host, "right");
    top = build_stack(host, "top");
    bottom = build_stack(host, "bottom");

    centre_right = join_regions(
        GTK_ORIENTATION_HORIZONTAL,
        centre,
        right,
        880,
        true,
        false);
    middle = join_regions(
        GTK_ORIENTATION_HORIZONTAL,
        left,
        centre_right,
        260,
        false,
        true);
    main_bottom = join_regions(
        GTK_ORIENTATION_VERTICAL,
        middle,
        bottom,
        620,
        true,
        false);
    workspace = join_regions(
        GTK_ORIENTATION_VERTICAL,
        top,
        main_bottom,
        180,
        false,
        true);

    if (workspace == NULL) {
        const size_t floating_count =
            visible_floating_window_count(layout);

        workspace = floating_count > 0U
            ? create_workspace_empty_state(
                  "Workspace panels are detached",
                  "Use Edit Layout to dock a panel into this window.")
            : create_workspace_empty_state(
                  "No visible panels",
                  "Use Edit Layout to add or restore workspace panels.");
    }
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    gtk_widget_set_hexpand(workspace, TRUE);
    gtk_widget_set_vexpand(workspace, TRUE);
    gtk_box_append(GTK_BOX(host->root), workspace);

    build_floating_windows(host);
    return UMI_STATUS_OK;
}

/* Borrow the stable root. Rebuilds change only its child tree. */
GtkWidget *umi_gtk4_workspace_layout_host_widget(
    UmiGtk4WorkspaceLayoutHost *host)
{
    return host != NULL ? host->root : NULL;
}

/* Count model windows represented by the current native host. */
size_t umi_gtk4_workspace_layout_host_window_count(
    const UmiGtk4WorkspaceLayoutHost *host)
{
    return host != NULL ? host->layout.window_count : 0U;
}
