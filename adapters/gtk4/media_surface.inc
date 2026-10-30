/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/media_surface.inc
 *
 * PURPOSE:
 *   Compose product-specific Media navigation over existing Framework editors without duplicating their state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static GtkWidget *MediaFind(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = MediaFind(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Selection follows the actual hosted page, including navigation from its
 * own notebook. Buttons never maintain a competing active-tool model. */
static bool MediaRouteSelected(GtkStack *pages, const char *route)
{
    const char *visible = gtk_stack_get_visible_child_name(pages);
    if (visible == NULL || route == NULL) return false;
    if (strcmp(visible, route) == 0) return true;
    if (strcmp(visible, "creative-workspace") != 0) return false;
    GtkWidget *page = MediaFind(GTK_WIDGET(pages), route);
    if (page == NULL) return false;
    GtkWidget *notebook = gtk_widget_get_ancestor(page, GTK_TYPE_NOTEBOOK);
    if (notebook == NULL) return false;
    int index = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
    if (index < 0) return false;
    GtkWidget *selected = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), index);
    return page == selected || gtk_widget_is_ancestor(page, selected);
}
static void MediaMarkNavigation(GtkWidget *widget, GtkStack *pages)
{
    const char *id = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (GTK_IS_BUTTON(widget) && id != NULL &&
        (g_str_has_prefix(id, "media.mode.") || g_str_has_prefix(id, "media.rail."))) {
        const char *route = g_object_get_data(G_OBJECT(widget), "media-route");
        if (MediaRouteSelected(pages, route)) gtk_widget_add_css_class(widget, "suggested-action");
        else gtk_widget_remove_css_class(widget, "suggested-action");
    }
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) MediaMarkNavigation(child, pages);
}
static void MediaNavigationChanged(GObject *source, GParamSpec *property, gpointer root)
{
    (void)source; (void)property;
    GtkWidget *pages = MediaFind(GTK_WIDGET(root), "creative.surface.pages");
    if (GTK_IS_STACK(pages)) MediaMarkNavigation(GTK_WIDGET(root), GTK_STACK(pages));
}
static void MediaBindNavigation(UmiCreativeGtkSurface *surface)
{
    g_signal_connect_object(surface->pages, "notify::visible-child",
        G_CALLBACK(MediaNavigationChanged), surface->root, 0);
    GtkWidget *page = MediaFind(GTK_WIDGET(surface->pages), "creative.tools.canvas");
    GtkWidget *notebook = page != NULL ? gtk_widget_get_ancestor(page, GTK_TYPE_NOTEBOOK) : NULL;
    if (notebook != NULL) g_signal_connect_object(notebook, "notify::page",
        G_CALLBACK(MediaNavigationChanged), surface->root, 0);
    MediaNavigationChanged(NULL, NULL, surface->root);
}

static void MediaRoute(GtkButton *button, gpointer data)
{
    if (!gtk_widget_is_sensitive(GTK_WIDGET(button))) return;
    GtkStack *pages = GTK_STACK(data);
    const char *route = g_object_get_data(G_OBJECT(button), "media-route");
    if (route == NULL) return;
    if (strcmp(route, "video-editing") == 0 || strcmp(route, "layouts") == 0) {
        gtk_stack_set_visible_child_name(pages, route);
        return;
    }
    if (g_str_has_prefix(route, "video.")) {
        /* Menus dispatch the original guarded command. They neither bypass
         * busy/retired checks nor implement a second save or render path. */
        GtkWidget *command = MediaFind(GTK_WIDGET(pages), route);
        if (GTK_IS_BUTTON(command) && gtk_widget_is_sensitive(command)) {
            gtk_stack_set_visible_child_name(pages, "video-editing");
            g_signal_emit_by_name(command, "clicked");
        }
        return;
    }
    GtkWidget *page = MediaFind(GTK_WIDGET(pages), route);
    if (page == NULL) return;
    GtkWidget *notebook = gtk_widget_get_ancestor(page, GTK_TYPE_NOTEBOOK);
    if (!GTK_IS_NOTEBOOK(notebook)) return;
    int count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    for (int index = 0; index < count; ++index) {
        GtkWidget *child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), index);
        if (child == page || gtk_widget_is_ancestor(page, child)) {
            gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), index);
            gtk_stack_set_visible_child_name(pages, "creative-workspace");
            return;
        }
    }
}

static GtkWidget *MediaRouteButton(GtkStack *pages, const char *label,
                                   const char *id, const char *route)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    g_object_set_data_full(G_OBJECT(button), "umicom-automation-id", g_strdup(id), g_free);
    g_object_set_data_full(G_OBJECT(button), "media-route", g_strdup(route), g_free);
    g_signal_connect_object(button, "clicked", G_CALLBACK(MediaRoute), pages, 0);
    return button;
}

static GtkWidget *MediaMenu(GtkWidget *bar, const char *label)
{
    GtkWidget *menu = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(menu), label);
    GtkWidget *popover = gtk_popover_new();
    GtkWidget *items = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_popover_set_child(GTK_POPOVER(popover), items);
    gtk_menu_button_set_popover(GTK_MENU_BUTTON(menu), popover);
    gtk_box_append(GTK_BOX(bar), menu);
    return items;
}

static void MediaMenuDismiss(GtkButton *button, gpointer unused)
{
    (void)unused;
    GtkWidget *popover = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_POPOVER);
    if (popover != NULL) gtk_popover_popdown(GTK_POPOVER(popover));
}

static void MediaMenuItem(GtkStack *pages, GtkWidget *menu, const char *label,
                          const char *id, const char *route)
{
    GtkWidget *item = MediaRouteButton(pages, label, id, route);
    g_signal_connect(item, "clicked", G_CALLBACK(MediaMenuDismiss), NULL);
    gtk_box_append(GTK_BOX(menu), item);
}

static void MediaHeader(UmiCreativeGtkSurface *surface)
{
    UmiMediaWorkspaceStyle(surface->root);
    GtkWidget *bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_add_css_class(bar, "media-brandbar");
    gtk_box_append(GTK_BOX(surface->root), bar);
    gtk_box_append(GTK_BOX(bar), UmiMediaWorkspaceBrand(true));
    GtkWidget *name = gtk_label_new("MEDIA");
    gtk_widget_add_css_class(name, "heading");
    gtk_box_append(GTK_BOX(bar), name);
    GtkWidget *menu = MediaMenu(bar, "File");
    MediaMenuItem(surface->pages, menu, "Project and recipe paths", "media.menu.project", "video.navigation.project");
    MediaMenuItem(surface->pages, menu, "Save new recipe", "media.menu.save", "video.save");
    MediaMenuItem(surface->pages, menu, "Export settings", "media.menu.export", "video.navigation.export");
    menu = MediaMenu(bar, "Edit");
    MediaMenuItem(surface->pages, menu, "Undo video edit", "media.menu.undo", "video.undo");
    MediaMenuItem(surface->pages, menu, "Redo video edit", "media.menu.redo", "video.redo");
    menu = MediaMenu(bar, "View");
    MediaMenuItem(surface->pages, menu, "Video editing", "media.menu.video", "video-editing");
    MediaMenuItem(surface->pages, menu, "Asset library", "media.menu.library", "creative.tools.library");
    MediaMenuItem(surface->pages, menu, "Workspace layouts", "media.menu.layouts", "layouts");
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_box_append(GTK_BOX(bar), spacer);
    gtk_box_append(GTK_BOX(bar), MediaRouteButton(surface->pages, "Export",
        "media.export", "video.navigation.export"));

    GtkWidget *modes = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(modes, "media-modebar");
    /* A horizontal scroller keeps commands reachable on narrower monitors,
     * without asking an invisible specialist page to size the video editor. */
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), modes);
    gtk_box_append(GTK_BOX(surface->root), scroll);
    const char *labels[] = {"Video Edit", "AI Video", "AI Image", "Audio", "Canvas", "Layouts"};
    const char *ids[] = {"video", "ai-video", "ai-image", "audio", "canvas", "layouts"};
    const char *routes[] = {"video-editing", "creative.tools.ai-video", "creative.tools.ai-image",
                            "creative.tools.audio", "creative.tools.canvas", "layouts"};
    for (size_t i = 0U; i < G_N_ELEMENTS(labels); ++i) {
        char *id = g_strdup_printf("media.mode.%s", ids[i]);
        gtk_box_append(GTK_BOX(modes), MediaRouteButton(surface->pages, labels[i], id, routes[i]));
        g_free(id);
    }
}

static GtkWidget *MediaRail(UmiCreativeGtkSurface *surface)
{
    GtkWidget *rail = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_add_css_class(rail, "media-rail");
    gtk_box_append(GTK_BOX(rail), UmiMediaWorkspaceBrand(false));
    g_object_set_data(G_OBJECT(rail), "umicom-automation-id", "media.activity-rail");
    const char *labels[] = {"Media", "Audio", "Captions", "Assets", "Canvas"};
    const char *icons[] = {"folder-videos-symbolic", "audio-x-generic-symbolic",
                          "insert-text-symbolic", "folder-pictures-symbolic", "applications-graphics-symbolic"};
    const char *routes[] = {"video-editing", "creative.tools.audio", "creative.tools.captions",
                            "creative.tools.library", "creative.tools.canvas"};
    for (size_t i = 0U; i < G_N_ELEMENTS(labels); ++i) {
        char *id = g_strdup_printf("media.rail.%zu", i);
        GtkWidget *button = MediaRouteButton(surface->pages, labels[i], id, routes[i]);
        g_free(id);
        GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        gtk_box_append(GTK_BOX(body), gtk_image_new_from_icon_name(icons[i]));
        gtk_box_append(GTK_BOX(body), gtk_label_new(labels[i]));
        gtk_button_set_child(GTK_BUTTON(button), body);
        gtk_accessible_update_property(GTK_ACCESSIBLE(button), GTK_ACCESSIBLE_PROPERTY_LABEL, labels[i], -1);
        gtk_box_append(GTK_BOX(rail), button);
    }
    return rail;
}
