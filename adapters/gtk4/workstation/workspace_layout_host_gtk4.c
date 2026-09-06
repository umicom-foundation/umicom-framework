/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_layout_host_gtk4.c
 *
 * PURPOSE:
 *   Render a complete portable workspace layout as GTK4 paned regions, tab
 *   groups, free-positioned canvas panels, floating windows and reusable
 *   Framework panel frames.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"
#include "umicom/ui/workbench_canvas.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/gtk4/workstation/panel_frame.h"
#include "umicom/ui/gtk4/workstation/shell_header.h"
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
    GtkWidget *frame;
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

/* Canvas gesture records are view-owned and never replace the layout model. */
typedef struct CanvasEntry CanvasEntry;

struct UmiGtk4WorkspaceLayoutHost {
    GtkWidget *root;
    UmiUiWorkspaceLayout layout;
    UmiApplicationSuiteLayoutRenderPlan plan;
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory;
    void *panel_user_data;
    UmiGtk4WorkspaceLayoutActionHandler action_handler;
    void *action_user_data;
    GtkWindow *transient_parent;
    GPtrArray *floating_windows;
    guint pending_action_id;
    GtkWidget *canvas_layer;
    GPtrArray *canvas_entries;
    CanvasEntry *active_canvas_entry;
    UmiGtk4WorkspaceCanvasGeometryHandler canvas_geometry_handler;
    void *canvas_geometry_user_data;
    guint pending_geometry_id;
    size_t placeholder_count;
    uint64_t revision;
    /* Optional body ownership keeps drafts alive while placement wrappers
     * change. Ordinary suite hosts keep their existing refresh behaviour. */
    GPtrArray *retained_content;
    int retain_content;
    int content_invalidated;
    const UmiUiWindowGroupStore *context_groups;
};

/* Defined with content ownership helpers below; native destruction must
 * separate a retained body from its disappearing frame before invalidation. */
static void retained_content_unmount_window(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id);

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
                pending->host->action_user_data);
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
    if (entry != NULL) {
        retained_content_unmount_window(entry->host, entry->window_id);
        /* The retained frame is still traversable even when GTK has already
         * removed it from an externally destroyed native window. */
        umi_gtk4_ws_panel_frame_invalidate_actions(entry->frame);
        umi_gtk4_ws_tab_host_invalidate_actions(entry->frame);
        entry->destroyed = 1;
    }
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
    if (entry == NULL || entry->host == NULL || entry->destroyed ||
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
    umi_gtk4_ws_panel_frame_invalidate_actions(entry->frame);
    umi_gtk4_ws_tab_host_invalidate_actions(entry->frame);
    if (entry->window != NULL) {
        if (entry->close_handler_id != 0U) {
            g_signal_handler_disconnect(
                entry->window, entry->close_handler_id);
        }
        if (entry->destroy_handler_id != 0U) {
            g_signal_handler_disconnect(
                entry->window, entry->destroy_handler_id);
        }
        if (!entry->destroyed) {
            gtk_window_destroy(entry->window);
        }
        g_object_unref(entry->window);
        entry->window = NULL;
    }
    if (entry->frame != NULL) g_object_unref(entry->frame);
    entry->frame = NULL;
    g_free(entry);
}

/* Forward semantic panel actions to the owning workstation. */
static void on_panel_action(
    UmiWsPanelAction action,
    const UmiWsPanelChrome *chrome,
    void *user_data)
{
    UmiGtk4WorkspaceLayoutHost *host =
        (UmiGtk4WorkspaceLayoutHost *)user_data;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host != NULL && host->action_handler != NULL && chrome != NULL &&
        chrome->panel_id[0] != '\0') {
        host->action_handler(
            chrome->panel_id, action, host->action_user_data);
    }
}

/* A docked tab close button uses the same semantic action as a floating panel
 * header, keeping GTK state subordinate to the portable workspace model. */
static void on_tab_close(const char *tab_id, void *user_data)
{
    UmiGtk4WorkspaceLayoutHost *host =
        (UmiGtk4WorkspaceLayoutHost *)user_data;

    if (host != NULL && host->action_handler != NULL &&
        tab_id != NULL && tab_id[0] != '\0') {
        host->action_handler(
            tab_id, UMI_WS_PANEL_ACTION_CLOSE, host->action_user_data);
    }
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
    /* Explicitly invalidate before removal: callers may retain old action
     * buttons beyond their former host's lifetime. */
    umi_gtk4_ws_panel_frame_invalidate_actions(root);
    umi_gtk4_ws_tab_host_invalidate_actions(root);
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
           strcmp(placement_id, "bottom") == 0 ||
           strcmp(placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT) == 0;
}

/* Unknown custom placements fall back to centre so no visible panel is lost. */
static bool window_matches_placement(
    const UmiUiWorkspaceWindow *window,
    const char *placement_id)
{
    if (window == NULL || placement_id == NULL) return false;
    if (strcmp(window->placement_id, placement_id) == 0) return true;
    /* Free-positioned canvas records have a dedicated renderer and must not
     * become centre tabs, even when a layout contains both kinds of panel. */
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

/*
 * Initialise gtk4 workspace layout placeholder from caller-provided values so later
 * operations receive a known state.
 */
GtkWidget *umi_gtk4_workspace_layout_placeholder_create(
    const char *title,
    const char *message)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *heading = gtk_label_new(title != NULL ? title : "Panel");
    GtkWidget *body = gtk_label_new(
        message != NULL ? message : "No frontend renderer is available.");

    if (box == NULL || heading == NULL || body == NULL) return box;
    gtk_widget_add_css_class(box, "umicom-workspace-placeholder");
    gtk_widget_add_css_class(heading, "title-4");
    gtk_widget_add_css_class(body, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(body), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(body), TRUE);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), body);
    return box;
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

#include "workspace_content_gtk4.inc"

/* Create content through the application factory or a truthful placeholder. */
static GtkWidget *create_content(
    UmiGtk4WorkspaceLayoutHost *host,
    const UmiUiWorkspaceWindow *window)
{
    GtkWidget *content = retained_content_create(host, window);

    /* Apply this branch only when its contract condition is satisfied. */
    if (content == NULL) {
        content = umi_gtk4_workspace_layout_placeholder_create(
            window->title,
            "This panel has no active provider.");
        host->placeholder_count += 1U;
    }
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_vexpand(content, TRUE);
    return content;
}

/* Resolve group identity through the existing routing store. A group ID is
 * not a colour token; using it as both previously hid most group stripes. */
static const char *window_context_colour(const UmiGtk4WorkspaceLayoutHost *host,
                                        const UmiUiWorkspaceWindow *window)
{
    const UmiUiWindowGroupStore *groups = host->context_groups;
    if (groups == NULL) return window->context_group_id;
    if (groups->count > UMI_UI_WINDOW_GROUP_MAX) return "";
    for (size_t index = 0U; index < groups->count; ++index) {
        const UmiUiWindowGroup *group = &groups->items[index];
        if (memchr(group->group_id, '\0', sizeof(group->group_id)) == NULL ||
            memchr(group->colour_token, '\0', sizeof(group->colour_token)) == NULL)
            continue;
        if (strcmp(group->group_id, window->context_group_id) == 0)
            return group->colour_token;
    }
    return "";
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
    if (!window->floating &&
        (host->retain_content ||
         strcmp(window->placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT) == 0) &&
        !GTK_IS_SCROLLED_WINDOW(content)) {
        GtkWidget *viewport = gtk_scrolled_window_new();

        /* Keep oversized provider minima from moving the panel chrome outside
         * its canvas or retained dock bounds. The provider remains available by scrolling;
         * providers that already own a scroller keep that existing viewport. */
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(viewport),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_propagate_natural_width(
            GTK_SCROLLED_WINDOW(viewport), FALSE);
        gtk_scrolled_window_set_propagate_natural_height(
            GTK_SCROLLED_WINDOW(viewport), FALSE);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(viewport), content);
        gtk_widget_set_hexpand(viewport, TRUE);
        gtk_widget_set_vexpand(viewport, TRUE);
        content = viewport;
    }
    (void)umi_ws_panel_chrome_init(&chrome, window->title);
    (void)umi_ws_panel_chrome_set_identity(
        &chrome, window->window_id, window->placement_id);
    (void)umi_ws_panel_chrome_set_context(
        &chrome, window->context_group_id, window_context_colour(host, window));
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
    /* Only native detached windows currently apply maximisation. Do not offer
     * an internal-panel action that merely changes a flag without resizing. */
    chrome.show_maximise = window->floating;
    chrome.show_settings = true;
    chrome.compact = true;
    chrome.pinned = window->pinned;
    chrome.locked = host->layout.locked;
    chrome.floating = window->floating;
    chrome.maximised = window->maximised;
    GtkWidget *frame = umi_gtk4_ws_panel_frame_create_interactive(
        &chrome, content, on_panel_action, host);
    /* A copied identity lets commands find the live frame after any rebuild,
     * without holding a pointer into an old notebook or canvas. */
    if (frame != NULL)
        g_object_set_data_full(G_OBJECT(frame), "umicom-workspace-window-id",
            g_strdup(window->window_id), g_free);
    return frame;
}

/* Create one notebook for every distinct tab stack in a dock region. Empty
 * regions return NULL so they do not consume permanent workspace space. */
static GtkWidget *build_stack(
    UmiGtk4WorkspaceLayoutHost *host,
    const char *placement_id)
{
    StackEntry entries[UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS];
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
            if (entry_count >= UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS) continue;
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

/* Keep the private native canvas implementation beside this adapter. It uses
 * the same panel factory, chrome and portable model rather than a second
 * layout engine. */
#include "workspace_canvas_gtk4.inc"

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
        entry->frame = frame;
        g_object_ref_sink(frame);
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
        /* Detached panels inherit the same canonical native identity as their
         * product window without replacing caller-controlled decorations. */
        (void)umi_gtk4_ws_apply_window_identity(window);
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
    const UmiUiWorkspaceLayout *layout,
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    void *user_data,
    UmiGtk4WorkspaceLayoutHost **out_host)
{
    return umi_gtk4_workspace_layout_host_create_interactive(
        layout,
        panel_factory,
        user_data,
        NULL,
        NULL,
        out_host);
}

/* Create an interactive host while preserving distinct panel and action data. */
UmiStatus umi_gtk4_workspace_layout_host_create_interactive(
    const UmiUiWorkspaceLayout *layout,
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory,
    void *panel_user_data,
    UmiGtk4WorkspaceLayoutActionHandler action_handler,
    void *action_user_data,
    UmiGtk4WorkspaceLayoutHost **out_host)
{
    UmiGtk4WorkspaceLayoutHost *host;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || out_host == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
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
    host->canvas_entries = g_ptr_array_new_with_free_func(canvas_entry_destroy);
    host->retained_content = g_ptr_array_new_with_free_func(retained_content_destroy);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host->root == NULL || host->floating_windows == NULL ||
        host->canvas_entries == NULL || host->retained_content == NULL) {
        umi_gtk4_workspace_layout_host_destroy(host);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    g_object_ref_sink(host->root);
    host->panel_factory = panel_factory;
    host->panel_user_data = panel_user_data;
    host->action_handler = action_handler;
    host->action_user_data = action_user_data;
    gtk_widget_set_hexpand(host->root, TRUE);
    gtk_widget_set_vexpand(host->root, TRUE);
    gtk_widget_add_css_class(host->root, "umicom-workspace-layout-host");
    (void)umi_gtk4_automation_tag_widget(
        host->root, "workstation.workspace-layout");

    status = umi_gtk4_workspace_layout_host_rebuild(host, layout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_gtk4_workspace_layout_host_destroy(host);
        return status;
    }
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
    cancel_canvas_geometry(host);
    /* Detach bodies before invalidating wrapper actions. A body can contain
     * its own controls, which are not part of the retired outer frame. */
    retained_content_unmount(host);
    if (host->retained_content != NULL) {
        g_ptr_array_free(host->retained_content, TRUE);
        host->retained_content = NULL;
    }
    clear_canvas_entries(host);
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
    if (host->canvas_entries != NULL)
        g_ptr_array_free(host->canvas_entries, TRUE);
    host->canvas_entries = NULL;
    if (host->root != NULL) {
        /* A caller may still parent or retain the stable root. Empty its old
         * action widgets before freeing the callback owner. */
        clear_root(host->root);
        g_object_unref(host->root);
        host->root = NULL;
    }
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
    UmiApplicationSuiteLayoutRenderPlan *candidate_plan;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    UmiStatus status;

    if (host == NULL || layout == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (layout->window_count > UMI_UI_WORKSPACE_LAYOUT_MAX_WINDOWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!workspace_layout_text_valid(layout)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Invalid replacement layouts must not overwrite counters belonging to
     * the currently rendered tree. Keep the bounded plan off the GTK stack
     * and publish it only after every input record has been validated. */
    candidate_plan = g_try_new0(UmiApplicationSuiteLayoutRenderPlan, 1);
    if (candidate_plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_application_suite_layout_render_plan_build(
        layout, candidate_plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        g_free(candidate_plan);
        return status;
    }
    status = retained_content_check_capacity(host, layout);
    if (status != UMI_STATUS_OK) {
        g_free(candidate_plan);
        return status;
    }
    host->plan = *candidate_plan;
    g_free(candidate_plan);
    cancel_canvas_geometry(host);
    if (host->pending_action_id != 0U) {
        (void)g_source_remove(host->pending_action_id);
        host->pending_action_id = 0U;
    }
    if (!host->content_invalidated && refresh_canvas_geometry_without_rebuild(host, layout))
        return UMI_STATUS_OK;
    retained_content_unmount(host);
    retained_content_prune(host, layout);
    clear_canvas_entries(host);
    host->layout = *layout;
    host->placeholder_count = 0U;
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

    if (workspace == NULL && host->plan.canvas_item_count == 0U) {
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
    workspace = build_canvas_layer(host, workspace);
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    gtk_widget_set_hexpand(workspace, TRUE);
    gtk_widget_set_vexpand(workspace, TRUE);
    gtk_box_append(GTK_BOX(host->root), workspace);

    build_floating_windows(host);
    host->revision += 1U;
    return UMI_STATUS_OK;
}

/* Borrow the stable root. Rebuilds change only its child tree. */
GtkWidget *umi_gtk4_workspace_layout_host_widget(
    UmiGtk4WorkspaceLayoutHost *host)
{
    return host != NULL ? host->root : NULL;
}

/*
 * Provide the gtk4 workspace layout host snapshot operation used by this module and its
 * client applications.
 */
UmiGtk4WorkspaceLayoutHostSnapshot umi_gtk4_workspace_layout_host_snapshot(
    const UmiGtk4WorkspaceLayoutHost *host)
{
    UmiGtk4WorkspaceLayoutHostSnapshot snapshot;

    (void)memset(&snapshot, 0, sizeof(snapshot));
    if (host == NULL) return snapshot;
    (void)snprintf(
        snapshot.layout_id, sizeof(snapshot.layout_id), "%s",
        host->layout.layout_id);
    snapshot.stack_count = host->plan.stack_count;
    snapshot.panel_count = host->plan.visible_window_count;
    snapshot.placeholder_count = host->placeholder_count;
    snapshot.floating_count = host->plan.floating_window_count;
    snapshot.revision = host->revision;
    snapshot.canvas_count = host->plan.canvas_item_count;
    snapshot.canvas_editable = !host->layout.locked &&
        host->canvas_geometry_handler != NULL;
    snapshot.geometry_pending = host->pending_geometry_id != 0U;
    snapshot.source_layout_revision = host->layout.revision;
    snapshot.content_retention_enabled = host->retain_content;
    snapshot.retained_content_count = host->retained_content != NULL
        ? host->retained_content->len : 0U;
    return snapshot;
}

/* Count model windows represented by the current native host. */
size_t umi_gtk4_workspace_layout_host_window_count(
    const UmiGtk4WorkspaceLayoutHost *host)
{
    return host != NULL ? host->layout.window_count : 0U;
}

/* Find only live frames below the current root. A retained old frame outside
 * this tree must never become the target of a later menu command. */
static GtkWidget *find_live_window_frame(GtkWidget *widget, const char *window_id)
{
    GtkWidget *child;
    const char *identity;
    if (widget == NULL) return NULL;
    identity = g_object_get_data(G_OBJECT(widget), "umicom-workspace-window-id");
    if (identity != NULL && strcmp(identity, window_id) == 0) return widget;
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_live_window_frame(child, window_id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Apply semantic group colours without rebuilding provider bodies or using
 * a saved group ID as an arbitrary CSS selector. The frame accepts only its
 * established palette tokens, including when layout data is user supplied. */
UmiStatus umi_gtk4_workspace_layout_host_set_context_groups(
    UmiGtk4WorkspaceLayoutHost *host, const UmiUiWindowGroupStore *groups)
{
    if (host == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (groups != NULL) {
        if (groups->count > UMI_UI_WINDOW_GROUP_MAX) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t index = 0U; index < groups->count; ++index) {
            if (memchr(groups->items[index].group_id, '\0', sizeof(groups->items[index].group_id)) == NULL ||
                memchr(groups->items[index].colour_token, '\0', sizeof(groups->items[index].colour_token)) == NULL)
                return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    host->context_groups = groups;
    for (size_t index = 0U; index < host->layout.window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &host->layout.windows[index];
        GtkWidget *frame;
        if (!window->visible) continue;
        frame = find_live_window_frame(host->root, window->window_id);
        if (window->floating) {
            for (guint floating = 0U; floating < host->floating_windows->len; ++floating) {
                FloatingWindowEntry *entry = g_ptr_array_index(host->floating_windows, floating);
                if (!entry->destroyed && strcmp(entry->window_id, window->window_id) == 0) {
                    frame = entry->frame;
                    break;
                }
            }
        }
        if (frame != NULL)
            umi_gtk4_ws_panel_frame_set_context_colour(frame, window_context_colour(host, window));
    }
    return UMI_STATUS_OK;
}

/* Activate the actual panel view without reopening hidden model instances or
 * changing its saved rectangle. This is also used by keyboard/menu navigation. */
UmiStatus umi_gtk4_workspace_layout_host_focus_window(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    const UmiUiWorkspaceWindow *window;
    GtkWidget *frame;
    GtkWidget *notebook;
    if (host == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    window = umi_ui_workspace_layout_find_window(&host->layout, window_id);
    if (window == NULL || !window->visible) return UMI_STATUS_NOT_FOUND;
    frame = find_live_window_frame(host->root, window_id);
    if (window->floating) {
        for (guint index = 0U; index < host->floating_windows->len; ++index) {
            FloatingWindowEntry *entry = g_ptr_array_index(host->floating_windows, index);
            if (!entry->destroyed && strcmp(entry->window_id, window_id) == 0) {
                frame = entry->frame;
                gtk_window_present(entry->window);
                break;
            }
        }
    }
    if (frame == NULL) return UMI_STATUS_NOT_FOUND;
    /* A notebook page is the frame supplied at construction, even though GTK
     * uses private containers between that frame and the notebook itself. */
    notebook = gtk_widget_get_ancestor(frame, GTK_TYPE_NOTEBOOK);
    if (notebook != NULL) {
        int page = gtk_notebook_page_num(GTK_NOTEBOOK(notebook), frame);
        if (page >= 0) gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page);
    }
    for (guint index = 0U; index < host->canvas_entries->len; ++index) {
        CanvasEntry *entry = g_ptr_array_index(host->canvas_entries, index);
        if (strcmp(entry->window_id, window_id) == 0 && host->canvas_layer != NULL) {
            GtkWidget *last = gtk_widget_get_last_child(host->canvas_layer);
            if (last != entry->panel)
                gtk_widget_insert_after(entry->panel, host->canvas_layer, last);
            break;
        }
    }
    /* Prefer a real editor or control. Read-only panels still expose a focus
     * target so keyboard users can tell which panel was selected. */
    if (!gtk_widget_child_focus(frame, GTK_DIR_TAB_FORWARD)) {
        gtk_widget_set_focusable(frame, TRUE);
        (void)gtk_widget_grab_focus(frame);
    }
    return UMI_STATUS_OK;
}
