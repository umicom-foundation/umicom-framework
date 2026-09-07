/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_window_titlebar_gtk4.c
 * Purpose: Check topmost canonical identity and native titlebar ownership.
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/shell_header.h"
#include "umicom/ui/appearance_catalogue.h"

#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; goto cleanup; \
    } \
} while (0)

/* Locate stable production widgets, not their translated captions. */
static GtkWidget *find_tag(GtkWidget *root, const char *id)
{
    GtkWidget *child;
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_tag(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* Confirm GTK supplies its genuine handle and control widgets. */
static size_t count_type(GtkWidget *root, GType type)
{
    GtkWidget *child;
    size_t count = G_TYPE_CHECK_INSTANCE_TYPE(root, type) ? 1U : 0U;
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        count += count_type(child, type);
    return count;
}

/* GtkHeaderBar packs application actions in its public end slot alongside
 * GTK window controls. Check ancestry without allocating or presenting it. */
static int is_in_titlebar_end(GtkWidget *root, GtkWidget *control)
{
    GtkWidget *child;
    if (GTK_IS_CENTER_BOX(root)) {
        GtkWidget *end = gtk_center_box_get_end_widget(GTK_CENTER_BOX(root));
        if (end != NULL && count_type(end, GTK_TYPE_WINDOW_CONTROLS) > 0U &&
            (control == end || gtk_widget_is_ancestor(control, end))) return 1;
    }
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        if (is_in_titlebar_end(child, control)) return 1;
    return 0;
}

/* The same GtkWindow close-request guard remains authoritative with CSD. */
static gboolean guard_close(GtkWindow *window, gpointer user_data)
{
    unsigned *calls = user_data;
    (void)window;
    *calls += 1U;
    return TRUE;
}

/* Build real GTK widgets but never realize, map or present a window. */
int main(void)
{
    GtkWindow *window = NULL;
    GtkWindow *other_window = NULL;
    GtkWidget *content;
    GtkWidget *bar = NULL;
    GtkWidget *new_window = NULL;
    GtkWidget *catalogue;
    GtkWidget *managed_close;
    GtkWidget *context;
    GtkWidget *icon;
    GtkWidget *label;
    GdkPaintable *paintable;
    GFile *expected_file = NULL;
    GFile *actual_file = NULL;
    UmiGtk4WorkstationWindowTitlebar *titlebar = NULL;
    UmiGtk4WorkstationWindowTitlebar *other = NULL;
    UmiGtk4WorkstationShellHeaderConfig config =
        umi_gtk4_ws_shell_header_config_default("org.umicom.studio", "Umicom Studio IDE");
    UmiGtk4WorkstationWindowTitlebarSnapshot snapshot;
    UmiUiAppearanceProfile appearance;
    unsigned close_calls = 0U;
    guint clicked_signal = 0U;
    gboolean blocked = FALSE;
    int failed = 0;
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;

    window = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(window);
    content = gtk_label_new("Original editor and workspace content");
    gtk_window_set_child(window, content);
    gtk_window_set_title(window, "Umicom Studio — notes.umicom * — Project Alpha");
    CHECK(umi_gtk4_ws_window_titlebar_create(window, &config, "Umicom Studio", &titlebar) == UMI_STATUS_OK);
    bar = g_object_ref(umi_gtk4_ws_window_titlebar_widget(titlebar));
    CHECK(GTK_IS_HEADER_BAR(bar));
    CHECK(gtk_window_get_titlebar(window) == bar);
    CHECK(gtk_window_get_child(window) == content);
    CHECK(!gtk_widget_is_ancestor(bar, content));
    CHECK(gtk_widget_get_root(bar) == GTK_ROOT(window));
    CHECK(gtk_header_bar_get_show_title_buttons(GTK_HEADER_BAR(bar)));
    CHECK(strcmp(gtk_header_bar_get_decoration_layout(GTK_HEADER_BAR(bar)), ":minimize,maximize,close") == 0);
    CHECK(count_type(bar, GTK_TYPE_WINDOW_HANDLE) >= 1U);
    CHECK(count_type(bar, GTK_TYPE_WINDOW_CONTROLS) >= 1U);
    CHECK(!gtk_widget_get_visible(GTK_WIDGET(window)) && !gtk_widget_get_realized(GTK_WIDGET(window)));
    CHECK(umi_gtk4_ws_window_titlebar_create(window, &config, "Umicom Studio", &other) == UMI_STATUS_INVALID_STATE);
    CHECK(other == NULL);

    context = find_tag(bar, "workstation.window-titlebar.context");
    icon = find_tag(bar, "workstation.identity.icon");
    label = find_tag(bar, "workstation.identity.title");
    CHECK(GTK_IS_LABEL(context) && GTK_IS_PICTURE(icon) && GTK_IS_LABEL(label));
    CHECK(gtk_widget_is_ancestor(icon, bar));
    CHECK(gtk_header_bar_get_title_widget(GTK_HEADER_BAR(bar)) == context);
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(label)), "Umicom Studio IDE") == 0);
    catalogue = find_tag(bar, "workstation.application.catalogue");
    new_window = find_tag(bar, "workstation.application.new-window");
    managed_close = find_tag(bar, "workstation.application.close");
    if (new_window != NULL) g_object_ref(new_window);
    CHECK(GTK_IS_MENU_BUTTON(catalogue) && GTK_IS_BUTTON(new_window));
    CHECK(gtk_widget_get_visible(catalogue) && gtk_widget_get_visible(new_window));
    CHECK(gtk_menu_button_get_popover(GTK_MENU_BUTTON(catalogue)) != NULL);
    CHECK(is_in_titlebar_end(bar, catalogue) && is_in_titlebar_end(bar, new_window));
    CHECK(!is_in_titlebar_end(bar, label));
    CHECK(GTK_IS_BUTTON(managed_close) && !gtk_widget_get_visible(managed_close));
    clicked_signal = g_signal_lookup("clicked", GTK_TYPE_BUTTON);
    CHECK(clicked_signal != 0U &&
        g_signal_has_handler_pending(new_window, clicked_signal, 0U, TRUE));
    snapshot = umi_gtk4_ws_window_titlebar_snapshot(titlebar);
    CHECK(snapshot.installed);
    CHECK(strcmp(snapshot.context, "notes.umicom * — Project Alpha") == 0);
    CHECK(strcmp(gtk_window_get_title(window), "Umicom Studio — notes.umicom * — Project Alpha") == 0);
    CHECK(gtk_label_get_single_line_mode(GTK_LABEL(context)));
    CHECK(gtk_label_get_ellipsize(GTK_LABEL(context)) == PANGO_ELLIPSIZE_MIDDLE);
    CHECK(gtk_label_get_max_width_chars(GTK_LABEL(context)) == 72);

    CHECK(umi_ui_appearance_catalogue_find("umicom-dark", &appearance) == UMI_STATUS_OK);
    CHECK(g_strlcpy(appearance.icon_resource, UMICOM_TEST_BRAND_ICON_PATH,
        sizeof(appearance.icon_resource)) < sizeof(appearance.icon_resource));
    CHECK(umi_gtk4_ws_window_titlebar_apply_appearance(titlebar, &appearance) == UMI_STATUS_OK);
    snapshot = umi_gtk4_ws_window_titlebar_snapshot(titlebar);
    CHECK(snapshot.icon_visible);
    paintable = gtk_picture_get_paintable(GTK_PICTURE(icon));
    CHECK(GTK_IS_ICON_PAINTABLE(paintable));
    CHECK(gdk_paintable_get_intrinsic_width(paintable) > 0 &&
        gdk_paintable_get_intrinsic_width(paintable) <= 18);
    CHECK(gdk_paintable_get_intrinsic_height(paintable) > 0 &&
        gdk_paintable_get_intrinsic_height(paintable) <= 18);
    expected_file = g_file_new_for_path(UMICOM_TEST_BRAND_ICON_PATH);
    actual_file = gtk_icon_paintable_get_file(GTK_ICON_PAINTABLE(paintable));
    CHECK(actual_file != NULL && g_file_equal(expected_file, actual_file));

    gtk_window_set_title(window, "Umicom Studio IDE — λ.umicom — Project Beta");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(context)), "λ.umicom — Project Beta") == 0);
    gtk_window_set_title(window, "Umicom Studio Tools — keep this entire title");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(context)), "Umicom Studio Tools — keep this entire title") == 0);
    gtk_window_set_title(window, "Umicom Studio IDE");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(context)), "") == 0);
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(label)), "Umicom Studio IDE") == 0);
    g_signal_connect(window, "close-request", G_CALLBACK(guard_close), &close_calls);
    g_signal_emit_by_name(window, "close-request", &blocked);
    CHECK(blocked && close_calls == 1U);
    CHECK(gtk_window_get_titlebar(window) == bar);
    CHECK(!gtk_widget_get_visible(GTK_WIDGET(window)));

    /* Controller-first teardown disconnects window and scale-factor callbacks,
     * even if callers retain the original native titlebar widgets. */
    umi_gtk4_ws_window_titlebar_destroy(titlebar);
    titlebar = NULL;
    gtk_window_set_title(window, "Umicom Studio — must not reach released state");
    g_object_notify(G_OBJECT(icon), "scale-factor");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(context)), "") == 0);
    /* Never emit a launch signal: a broken teardown must not start a process.
     * Observing handler removal proves the retained button no longer borrows
     * the released controller without exercising a real application launch. */
    CHECK(!g_signal_has_handler_pending(new_window, clicked_signal, 0U, TRUE));

    /* Window-first teardown also works: no strong window/controller cycle. */
    other_window = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(other_window);
    CHECK(umi_gtk4_ws_window_titlebar_create(other_window, &config, NULL, &other) == UMI_STATUS_OK);
    gtk_window_destroy(other_window);
    g_object_unref(other_window);
    other_window = NULL;
    CHECK(!umi_gtk4_ws_window_titlebar_snapshot(other).installed);
cleanup:
    umi_gtk4_ws_window_titlebar_destroy(other);
    umi_gtk4_ws_window_titlebar_destroy(titlebar);
    if (other_window != NULL) { gtk_window_destroy(other_window); g_object_unref(other_window); }
    if (window != NULL) {
        g_signal_handlers_disconnect_by_data(window, &close_calls);
        gtk_window_destroy(window);
        g_object_unref(window);
    }
    if (bar != NULL) g_object_unref(bar);
    if (new_window != NULL) g_object_unref(new_window);
    if (expected_file != NULL) g_object_unref(expected_file);
    if (actual_file != NULL) g_object_unref(actual_file);
    return failed;
}
