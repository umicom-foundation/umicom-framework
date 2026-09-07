/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_tool_windows_gtk4.inc
 * Purpose: Render named edge tabs and transient, reusable tool-window flyouts.
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* Preferred flyout size is not a minimum size. GtkOverlay may allocate this
 * private widget below 300 by 230 logical pixels, keeping its real controls
 * inside the available workspace instead of cropping an oversized GtkBox. */
typedef struct UmiWorkspaceToolFlyout {
    GtkWidget parent_instance;
    GtkWidget *child;
} UmiWorkspaceToolFlyout;
typedef struct UmiWorkspaceToolFlyoutClass {
    GtkWidgetClass parent_class;
} UmiWorkspaceToolFlyoutClass;

/* The flyout is a native presentation wrapper, not another layout model. */
static GType umi_workspace_tool_flyout_get_type(void);
G_DEFINE_TYPE(UmiWorkspaceToolFlyout, umi_workspace_tool_flyout, GTK_TYPE_WIDGET)

/* Let the overlay shrink either dimension without inheriting provider minima. */
static void tool_flyout_measure(GtkWidget *widget, GtkOrientation orientation,
    int for_size, int *minimum, int *natural,
    int *minimum_baseline, int *natural_baseline)
{
    (void)widget;
    (void)for_size;
    if (minimum != NULL) *minimum = 0;
    if (natural != NULL)
        *natural = orientation == GTK_ORIENTATION_HORIZONTAL ? 300 : 230;
    if (minimum_baseline != NULL) *minimum_baseline = -1;
    if (natural_baseline != NULL) *natural_baseline = -1;
}

/* Header labels ellipsize and provider content scrolls within this allocation. */
static void tool_flyout_size_allocate(GtkWidget *widget, int width, int height,
    int baseline)
{
    UmiWorkspaceToolFlyout *flyout = (UmiWorkspaceToolFlyout *)widget;
    (void)baseline;
    if (flyout->child != NULL)
        gtk_widget_allocate(flyout->child, MAX(0, width), MAX(0, height), -1, NULL);
}

/* Keep flyout painting within its own bounded allocation on every edge. */
static void tool_flyout_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    UmiWorkspaceToolFlyout *flyout = (UmiWorkspaceToolFlyout *)widget;
    const graphene_rect_t bounds = GRAPHENE_RECT_INIT(0.0F, 0.0F,
        (float)gtk_widget_get_width(widget), (float)gtk_widget_get_height(widget));
    if (flyout->child == NULL) return;
    gtk_snapshot_push_clip(snapshot, &bounds);
    gtk_widget_snapshot_child(widget, flyout->child, snapshot);
    gtk_snapshot_pop(snapshot);
}

/* Release only the child; host callback records are invalidated separately. */
static void tool_flyout_dispose(GObject *object)
{
    UmiWorkspaceToolFlyout *flyout = (UmiWorkspaceToolFlyout *)object;
    if (flyout->child != NULL) {
        gtk_widget_unparent(flyout->child);
        flyout->child = NULL;
    }
    G_OBJECT_CLASS(umi_workspace_tool_flyout_parent_class)->dispose(object);
}

/* Use standard GtkWidget allocation and child ownership contracts. */
static void umi_workspace_tool_flyout_class_init(UmiWorkspaceToolFlyoutClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    G_OBJECT_CLASS(klass)->dispose = tool_flyout_dispose;
    widget_class->measure = tool_flyout_measure;
    widget_class->size_allocate = tool_flyout_size_allocate;
    widget_class->snapshot = tool_flyout_snapshot;
    gtk_widget_class_set_css_name(widget_class, "umicom-tool-flyout");
}

/* Overflow is clipped for both picking and painting, not just snapshots. */
static void umi_workspace_tool_flyout_init(UmiWorkspaceToolFlyout *flyout)
{
    gtk_widget_set_overflow(GTK_WIDGET(flyout), GTK_OVERFLOW_HIDDEN);
}

/* Adopt one ordinary GTK body while keeping its preferred size shrinkable. */
static GtkWidget *tool_flyout_new(GtkWidget *child)
{
    UmiWorkspaceToolFlyout *flyout =
        g_object_new(umi_workspace_tool_flyout_get_type(), NULL);
    flyout->child = child;
    gtk_widget_set_parent(child, GTK_WIDGET(flyout));
    return GTK_WIDGET(flyout);
}

/* A tab owns only its callback and button reference. Its page belongs to the
 * edge stack; retained provider bodies continue to belong to the host cache. */
typedef struct ToolWindowTab {
    UmiGtk4WorkspaceLayoutHost *host;
    int edge_index;
    GtkWidget *button;
    GtkWidget *page;
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
} ToolWindowTab;

static const char *const tool_edge_names[4] = {"left", "right", "top", "bottom"};

/* Read bounded model identities, never labels, when resolving a tool. */
static const UmiUiWorkspaceWindow *tool_window_record(
    const UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    for (size_t index = 0U; index < host->layout.window_count; ++index)
        if (strcmp(host->layout.windows[index].window_id, window_id) == 0)
            return &host->layout.windows[index];
    return NULL;
}

/* Locate the current callback record without borrowing an old layout pointer. */
static ToolWindowTab *tool_window_tab(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    if (host->tool_window_tabs == NULL) return NULL;
    for (guint index = 0U; index < host->tool_window_tabs->len; ++index) {
        ToolWindowTab *tab = g_ptr_array_index(host->tool_window_tabs, index);
        if (strcmp(tab->window_id, window_id) == 0) return tab;
    }
    return NULL;
}

/* Disconnect before releasing the callback record. A test, accessibility
 * client or another widget may still own the old button after a rebuild. */
static void tool_window_tab_destroy(gpointer data)
{
    ToolWindowTab *tab = data;
    if (tab == NULL) return;
    if (tab->button != NULL) {
        g_signal_handlers_disconnect_by_data(tab->button, tab);
        g_object_unref(tab->button);
    }
    g_free(tab);
}

/* A collapsed tool is not closed in the model. Switching pages never removes
 * a provider widget or loses text entered into it. */
void umi_gtk4_workspace_layout_host_collapse_tool_windows(
    UmiGtk4WorkspaceLayoutHost *host)
{
    if (host == NULL) return;
    host->updating_tool_tabs = 1;
    if (host->tool_window_tabs != NULL) {
        for (guint index = 0U; index < host->tool_window_tabs->len; ++index) {
            ToolWindowTab *tab = g_ptr_array_index(host->tool_window_tabs, index);
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tab->button), FALSE);
        }
    }
    for (int index = 0; index < 4; ++index) {
        ToolWindowEdge *edge = &host->tool_edges[index];
        edge->selected_id[0] = '\0';
        if (edge->drawer != NULL)
            gtk_revealer_set_reveal_child(GTK_REVEALER(edge->drawer), FALSE);
    }
    host->updating_tool_tabs = 0;
}

/* Explicit reveal is separate from periodic focus restoration. Only a user
 * selecting a tab or a menu entry should open a previously collapsed flyout. */
UmiStatus umi_gtk4_workspace_layout_host_reveal_tool_window(
    UmiGtk4WorkspaceLayoutHost *host, const char *window_id)
{
    ToolWindowTab *tab;
    ToolWindowEdge *edge;
    const UmiUiWorkspaceWindow *window;
    if (host == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    tab = tool_window_tab(host, window_id);
    if (tab == NULL) return UMI_STATUS_NOT_FOUND;
    window = tool_window_record(host, window_id);
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    restore_maximised_presentation(host, 0);
    edge = &host->tool_edges[tab->edge_index];
    if (tab->page == NULL) {
        GtkWidget *content = create_content(host, window);
        GtkWidget *viewport = gtk_scrolled_window_new();
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(viewport),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_propagate_natural_width(
            GTK_SCROLLED_WINDOW(viewport), FALSE);
        gtk_scrolled_window_set_propagate_natural_height(
            GTK_SCROLLED_WINDOW(viewport), FALSE);
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(viewport), content);
        gtk_widget_set_hexpand(viewport, TRUE);
        gtk_widget_set_vexpand(viewport, TRUE);
        gtk_stack_add_named(GTK_STACK(edge->stack), viewport, window_id);
        g_object_set_data_full(G_OBJECT(viewport), "umicom-workspace-window-id",
            g_strdup(window_id), g_free);
        tab->page = viewport;
    }
    umi_gtk4_workspace_layout_host_collapse_tool_windows(host);
    (void)snprintf(edge->selected_id, sizeof(edge->selected_id), "%s", window_id);
    gtk_label_set_text(GTK_LABEL(edge->title),
        window->title[0] != '\0' ? window->title : window->window_id);
    gtk_widget_set_sensitive(edge->dock_button,
        host->tool_presentation_actions && host->action_handler != NULL && !window->pinned);
    gtk_stack_set_visible_child(GTK_STACK(edge->stack), tab->page);
    gtk_revealer_set_reveal_child(GTK_REVEALER(edge->drawer), TRUE);
    host->updating_tool_tabs = 1;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tab->button), TRUE);
    host->updating_tool_tabs = 0;
    (void)gtk_widget_child_focus(tab->page, GTK_DIR_TAB_FORWARD);
    return UMI_STATUS_OK;
}

/* A user toggle peeks or collapses; programmatic selection is signal-guarded. */
static void on_tool_window_tab_toggled(GtkToggleButton *button, gpointer data)
{
    ToolWindowTab *tab = data;
    if (tab == NULL || tab->host->updating_tool_tabs) return;
    if (gtk_toggle_button_get_active(button))
        (void)umi_gtk4_workspace_layout_host_reveal_tool_window(
            tab->host, tab->window_id);
    else if (strcmp(tab->host->tool_edges[tab->edge_index].selected_id,
                    tab->window_id) == 0)
        umi_gtk4_workspace_layout_host_collapse_tool_windows(tab->host);
}

/* X collapses the flyout, leaving its tab and cached provider available. */
static void on_tool_window_close(GtkButton *button, gpointer data)
{
    ToolWindowEdge *edge = data;
    (void)button;
    umi_gtk4_workspace_layout_host_collapse_tool_windows(edge->host);
}

/* Docking is a portable model command. Defer it until this signal returns so
 * the owner may safely rebuild every wrapper or even destroy the host. */
static void on_tool_window_dock(GtkButton *button, gpointer data)
{
    ToolWindowEdge *edge = data;
    UmiGtk4WorkspaceLayoutHost *host = edge->host;
    PendingHostAction *pending;
    const UmiUiWorkspaceWindow *window;
    (void)button;
    if (!host->tool_presentation_actions || host->pending_action_id != 0U || host->action_handler == NULL ||
        edge->selected_id[0] == '\0') return;
    window = tool_window_record(host, edge->selected_id);
    if (window == NULL || window->pinned) return;
    pending = g_new0(PendingHostAction, 1);
    pending->host = host;
    pending->action = UMI_WS_PANEL_ACTION_AUTO_HIDE_TOGGLE;
    (void)snprintf(pending->panel_id, sizeof(pending->panel_id),
        "%s", edge->selected_id);
    host->pending_action_id = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE,
        dispatch_host_action_from_idle, pending, g_free);
    if (host->pending_action_id == 0U) g_free(pending);
}

/* Escape hides only the temporary flyout. The owning editor and all saved
 * geometry are untouched, and an unrelated Escape keeps its usual meaning. */
static gboolean on_tool_window_key(GtkEventControllerKey *controller,
    guint keyval, guint keycode, GdkModifierType modifiers, gpointer data)
{
    UmiGtk4WorkspaceLayoutHost *host = data;
    (void)controller; (void)keycode; (void)modifiers;
    if (keyval != GDK_KEY_Escape) return FALSE;
    for (int edge = 0; edge < 4; ++edge) {
        if (host->tool_edges[edge].selected_id[0] != '\0') {
            umi_gtk4_workspace_layout_host_collapse_tool_windows(host);
            return TRUE;
        }
    }
    return FALSE;
}

/* Observe an editor click without consuming it. Rail/header clicks are inside
 * their own widgets, so they keep the currently selected tool available. */
static void on_tool_window_outside_click(GtkGestureClick *gesture,
    int presses, double x, double y, gpointer data)
{
    UmiGtk4WorkspaceLayoutHost *host = data;
    GtkWidget *picked = gtk_widget_pick(host->root, x, y, GTK_PICK_DEFAULT);
    (void)gesture; (void)presses;
    if (picked == NULL) return;
    for (int index = 0; index < 4; ++index) {
        ToolWindowEdge *edge = &host->tool_edges[index];
        if ((edge->drawer != NULL && (picked == edge->drawer ||
             gtk_widget_is_ancestor(picked, edge->drawer))) ||
            (edge->rail != NULL && (picked == edge->rail ||
             gtk_widget_is_ancestor(picked, edge->rail)))) return;
    }
    umi_gtk4_workspace_layout_host_collapse_tool_windows(host);
}

/* Invalidate callbacks while their widgets are still alive; only then may
 * clear_root release the old tree. Strong header refs cover externally
 * retained descendants just as the normal panel-frame API does. */
static void clear_tool_windows(UmiGtk4WorkspaceLayoutHost *host)
{
    if (host->tool_window_tabs != NULL)
        g_ptr_array_set_size(host->tool_window_tabs, 0);
    for (int index = 0; index < 4; ++index) {
        ToolWindowEdge *edge = &host->tool_edges[index];
        if (edge->close_button != NULL) {
            g_signal_handlers_disconnect_by_data(edge->close_button, edge);
            g_object_unref(edge->close_button);
        }
        if (edge->dock_button != NULL) {
            g_signal_handlers_disconnect_by_data(edge->dock_button, edge);
            g_object_unref(edge->dock_button);
        }
        (void)memset(edge, 0, sizeof(*edge));
    }
    if (host->tool_keys != NULL) {
        g_signal_handlers_disconnect_by_data(host->tool_keys, host);
        gtk_widget_remove_controller(host->root, host->tool_keys);
        host->tool_keys = NULL;
    }
    if (host->tool_outside_click != NULL) {
        g_signal_handlers_disconnect_by_data(host->tool_outside_click, host);
        gtk_widget_remove_controller(host->root,
            GTK_EVENT_CONTROLLER(host->tool_outside_click));
        host->tool_outside_click = NULL;
    }
    host->tool_overlay = NULL;
}

/* Short stable automation IDs contain the edge, not localised title text. */
static void tag_tool_edge(GtkWidget *widget, const char *part, int index)
{
    char identity[96];
    (void)snprintf(identity, sizeof(identity), "workstation.tool-%s.%s",
        part, tool_edge_names[index]);
    (void)umi_gtk4_automation_tag_widget(widget, identity);
}

/* Overlay flyouts do not contribute a permanent editor minimum width. Their
 * scrolled viewports contain oversized provider widgets within the host. */
static void build_tool_edge(UmiGtk4WorkspaceLayoutHost *host, int index)
{
    ToolWindowEdge *edge = &host->tool_edges[index];
    const gboolean vertical = index < 2;
    GtkWidget *rail_box = umi_gtk4_ws_tool_rail_create(
        vertical ? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL);
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    edge->host = host;
    edge->rail = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(edge->rail),
        vertical ? GTK_POLICY_NEVER : GTK_POLICY_AUTOMATIC,
        vertical ? GTK_POLICY_AUTOMATIC : GTK_POLICY_NEVER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(edge->rail), rail_box);
    g_object_set_data(G_OBJECT(edge->rail), "umicom-tool-rail-box", rail_box);
    edge->drawer = gtk_revealer_new();
    /* Instant reveal keeps hidden overlays out of hit testing and avoids
     * animation timing changing automation or keyboard behaviour. */
    gtk_revealer_set_transition_type(GTK_REVEALER(edge->drawer),
        GTK_REVEALER_TRANSITION_TYPE_NONE);
    gtk_widget_set_halign(edge->drawer, vertical
        ? (index == 0 ? GTK_ALIGN_START : GTK_ALIGN_END) : GTK_ALIGN_FILL);
    gtk_widget_set_valign(edge->drawer, vertical ? GTK_ALIGN_FILL
        : (index == 2 ? GTK_ALIGN_START : GTK_ALIGN_END));
    gtk_widget_add_css_class(body, "umicom-tool-window-flyout");
    gtk_widget_add_css_class(body, "background");
    gtk_widget_add_css_class(header, "umicom-panel-header");
    gtk_widget_add_css_class(header, "umicom-panel-header-compact");
    edge->title = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(edge->title), 0.0F);
    gtk_label_set_ellipsize(GTK_LABEL(edge->title), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(edge->title, TRUE);
    edge->dock_button = gtk_button_new_from_icon_name("view-pin-symbolic");
    edge->close_button = gtk_button_new_from_icon_name("window-close-symbolic");
    gtk_widget_add_css_class(edge->dock_button, "umicom-panel-action");
    gtk_widget_add_css_class(edge->close_button, "umicom-panel-action");
    gtk_widget_add_css_class(edge->dock_button, "flat");
    gtk_widget_add_css_class(edge->close_button, "flat");
    gtk_accessible_update_property(GTK_ACCESSIBLE(edge->dock_button),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Dock this tool window", -1);
    gtk_accessible_update_property(GTK_ACCESSIBLE(edge->close_button),
        GTK_ACCESSIBLE_PROPERTY_LABEL, "Collapse this tool window", -1);
    g_object_ref_sink(edge->dock_button);
    g_object_ref_sink(edge->close_button);
    gtk_widget_set_tooltip_text(edge->dock_button, "Dock this tool window");
    gtk_widget_set_tooltip_text(edge->close_button, "Collapse this tool window");
    tag_tool_edge(edge->title, "title", index);
    tag_tool_edge(edge->dock_button, "dock", index);
    tag_tool_edge(edge->close_button, "close", index);
    g_signal_connect(edge->close_button, "clicked",
        G_CALLBACK(on_tool_window_close), edge);
    g_signal_connect(edge->dock_button, "clicked",
        G_CALLBACK(on_tool_window_dock), edge);
    gtk_box_append(GTK_BOX(header), edge->title);
    gtk_box_append(GTK_BOX(header), edge->dock_button);
    gtk_box_append(GTK_BOX(header), edge->close_button);
    edge->stack = gtk_stack_new();
    gtk_stack_set_hhomogeneous(GTK_STACK(edge->stack), FALSE);
    gtk_stack_set_vhomogeneous(GTK_STACK(edge->stack), FALSE);
    gtk_box_append(GTK_BOX(body), header);
    gtk_box_append(GTK_BOX(body), edge->stack);
    GtkWidget *flyout = tool_flyout_new(body);
    tag_tool_edge(flyout, "flyout", index);
    tag_tool_edge(edge->drawer, "drawer", index);
    gtk_revealer_set_child(GTK_REVEALER(edge->drawer), flyout);
    gtk_overlay_add_overlay(GTK_OVERLAY(host->tool_overlay), edge->drawer);
    gtk_overlay_set_measure_overlay(GTK_OVERLAY(host->tool_overlay),
        edge->drawer, FALSE);
    gtk_overlay_set_clip_overlay(GTK_OVERLAY(host->tool_overlay),
        edge->drawer, TRUE);
    tag_tool_edge(edge->rail, "rail", index);
}

/* Render only saved auto-hide members around the central workspace. A blank
 * or ordinary hidden layout therefore acquires no unintended edge controls. */
static GtkWidget *build_tool_windows(
    UmiGtk4WorkspaceLayoutHost *host, GtkWidget *workspace)
{
    GtkWidget *vertical;
    GtkWidget *horizontal;
    size_t count = 0U;
    for (size_t index = 0U; index < host->layout.window_count; ++index)
        if (tool_window_edge_index(&host->layout.windows[index]) >= 0) ++count;
    if (count == 0U) return workspace;
    vertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    horizontal = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(horizontal, TRUE);
    gtk_widget_set_vexpand(horizontal, TRUE);
    host->tool_overlay = gtk_overlay_new();
    (void)umi_gtk4_automation_tag_widget(host->tool_overlay, "workstation.tool-overlay");
    gtk_overlay_set_child(GTK_OVERLAY(host->tool_overlay), workspace);
    gtk_widget_set_hexpand(host->tool_overlay, TRUE);
    gtk_widget_set_vexpand(host->tool_overlay, TRUE);
    for (size_t index = 0U; index < host->layout.window_count; ++index) {
        const UmiUiWorkspaceWindow *window = &host->layout.windows[index];
        const int edge_index = tool_window_edge_index(window);
        ToolWindowTab *tab;
        ToolWindowEdge *edge;
        GtkWidget *rail_box;
        char identity[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 32];
        if (edge_index < 0) continue;
        edge = &host->tool_edges[edge_index];
        if (edge->rail == NULL) build_tool_edge(host, edge_index);
        rail_box = g_object_get_data(G_OBJECT(edge->rail), "umicom-tool-rail-box");
        tab = g_new0(ToolWindowTab, 1);
        tab->host = host;
        tab->edge_index = edge_index;
        (void)snprintf(tab->window_id, sizeof(tab->window_id),
            "%s", window->window_id);
        tab->button = umi_gtk4_ws_tool_rail_append_label(
            rail_box, window->title[0] != '\0' ? window->title : window->window_id,
            edge_index < 2, FALSE);
        if (tab->button == NULL) { g_free(tab); continue; }
        g_object_ref(tab->button);
        (void)snprintf(identity, sizeof(identity),
            "workstation.tool-tab.%s", window->window_id);
        (void)umi_gtk4_automation_tag_widget(tab->button, identity);
        g_signal_connect(tab->button, "toggled",
            G_CALLBACK(on_tool_window_tab_toggled), tab);
        g_ptr_array_add(host->tool_window_tabs, tab);
    }
    if (host->tool_edges[2].rail != NULL)
        gtk_box_append(GTK_BOX(vertical), host->tool_edges[2].rail);
    if (host->tool_edges[0].rail != NULL)
        gtk_box_append(GTK_BOX(horizontal), host->tool_edges[0].rail);
    gtk_box_append(GTK_BOX(horizontal), host->tool_overlay);
    if (host->tool_edges[1].rail != NULL)
        gtk_box_append(GTK_BOX(horizontal), host->tool_edges[1].rail);
    gtk_box_append(GTK_BOX(vertical), horizontal);
    if (host->tool_edges[3].rail != NULL)
        gtk_box_append(GTK_BOX(vertical), host->tool_edges[3].rail);
    host->tool_keys = gtk_event_controller_key_new();
    gtk_event_controller_set_propagation_phase(host->tool_keys, GTK_PHASE_CAPTURE);
    g_signal_connect(host->tool_keys, "key-pressed",
        G_CALLBACK(on_tool_window_key), host);
    gtk_widget_add_controller(host->root, host->tool_keys);
    host->tool_outside_click = gtk_gesture_click_new();
    gtk_event_controller_set_propagation_phase(
        GTK_EVENT_CONTROLLER(host->tool_outside_click), GTK_PHASE_CAPTURE);
    g_signal_connect(host->tool_outside_click, "pressed",
        G_CALLBACK(on_tool_window_outside_click), host);
    gtk_widget_add_controller(host->root,
        GTK_EVENT_CONTROLLER(host->tool_outside_click));
    return vertical;
}
