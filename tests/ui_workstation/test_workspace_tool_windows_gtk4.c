/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_workspace_tool_windows_gtk4.c
 *
 * PURPOSE:
 *   Exercise real shared edge tabs and tool drawers without presenting a
 *   window. Temporary reveal state must preserve editor drafts and must not
 *   mutate the portable workspace or dispatch after its host is released.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Acceptance checks must remain active when the application is built for release. */
#define REQUIRE(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; \
        goto cleanup; \
    } \
} while (0)

typedef struct ToolWindowFixture {
    GtkWidget *editor;
    GtkWidget *structure;
    GtkWidget *inspector;
    size_t created;
    size_t released;
    unsigned actions;
    UmiWsPanelAction last_action;
    char last_window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
} ToolWindowFixture;

/* Count final releases rather than ordinary unparenting during panel switches. */
static void tool_body_released(gpointer user_data, GObject *object)
{
    ToolWindowFixture *fixture = user_data;
    (void)object;
    ++fixture->released;
}

/* Real editable widgets make draft retention observable without files or tools. */
static GtkWidget *create_tool_body(const UmiUiWorkspaceWindow *window, void *user_data)
{
    ToolWindowFixture *fixture = user_data;
    GtkWidget *entry = gtk_entry_new();
    if (strcmp(window->window_id, "editor") == 0) fixture->editor = entry;
    else if (strcmp(window->window_id, "structure") == 0) fixture->structure = entry;
    else if (strcmp(window->window_id, "inspector") == 0) fixture->inspector = entry;
    g_object_weak_ref(G_OBJECT(entry), tool_body_released, fixture);
    ++fixture->created;
    return entry;
}

/* Dock requests cross the normal owner boundary; revealing or collapsing a
 * drawer must never reach this model-changing callback. */
static void record_tool_action(const char *window_id, UmiWsPanelAction action, void *user_data)
{
    ToolWindowFixture *fixture = user_data;
    ++fixture->actions;
    fixture->last_action = action;
    (void)g_strlcpy(fixture->last_window_id, window_id, sizeof(fixture->last_window_id));
}

/* Use stable production tags rather than captions or private child ordering. */
static GtkWidget *find_tool_control(GtkWidget *widget, const char *id)
{
    GtkWidget *child;
    const char *tag;
    if (widget == NULL) return NULL;
    tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return widget;
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_tool_control(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* An unpresented tree is never mapped. Check visible and child-visible policy
 * up to the host instead, so hidden drawers cannot pass as visible content. */
static int visible_in_host(GtkWidget *widget, GtkWidget *root)
{
    while (widget != NULL) {
        if (!gtk_widget_get_visible(widget) || !gtk_widget_get_child_visible(widget)) return 0;
        if (widget == root) return 1;
        widget = gtk_widget_get_parent(widget);
    }
    return 0;
}

/* Drain only currently ready local callbacks; never wait for a timer or display. */
static void drain_tool_callbacks(void)
{
    unsigned iteration;
    for (iteration = 0U; iteration < 64U; ++iteration) {
        if (!g_main_context_iteration(NULL, FALSE)) break;
    }
}

/* Fixtures use the portable API while unlocked and present a locked model to
 * the host. Auto-hide is explicit metadata, not synonymous with every hidden tool. */
static UmiStatus add_tool_window(UmiUiWorkspaceLayout *layout, const char *id,
    const char *title, const char *placement, bool visible)
{
    UmiUiWorkspaceWindow window = {0};
    if (g_strlcpy(window.window_id, id, sizeof(window.window_id)) >= sizeof(window.window_id) ||
        g_strlcpy(window.title, title, sizeof(window.title)) >= sizeof(window.title) ||
        g_strlcpy(window.placement_id, placement, sizeof(window.placement_id)) >= sizeof(window.placement_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)g_strlcpy(window.tool_id, "test.editable-tool", sizeof(window.tool_id));
    (void)g_strlcpy(window.stack_id, "test.tools", sizeof(window.stack_id));
    window.x = 0.0; window.y = 0.0; window.width = 0.25; window.height = 1.0;
    window.visible = visible;
    window.closable = true;
    window.resizable = true;
    return umi_ui_workspace_layout_add_window(layout, &window);
}

/* The host remains the coordinate authority even without a native top-level.
 * Allow sub-pixel rounding, but not a clipped or unreachable header control. */
static int tool_widget_fits(GtkWidget *widget, GtkWidget *bounds_owner)
{
    graphene_rect_t bounds;
    if (widget == NULL || bounds_owner == NULL ||
        !gtk_widget_compute_bounds(widget, bounds_owner, &bounds)) return 0;
    return bounds.origin.x >= -0.5F && bounds.origin.y >= -0.5F &&
        bounds.size.width > 0.0F && bounds.size.height > 0.0F &&
        bounds.origin.x + bounds.size.width <= (float)gtk_widget_get_width(bounds_owner) + 0.5F &&
        bounds.origin.y + bounds.size.height <= (float)gtk_widget_get_height(bounds_owner) + 0.5F;
}

/* Older owners and read-only viewers may peek at tools, but cannot gain new
 * persistent layout actions by receiving this native renderer upgrade. Mode2
 * also checks that options cannot grant authority when no owner callback exists. */
static int verify_legacy_tool_policy(const UmiUiWorkspaceLayout *source, unsigned mode)
{
    ToolWindowFixture fixture = {0};
    UmiUiWorkspaceLayout *layout = NULL;
    UmiGtk4WorkspaceLayoutHost *host = NULL;
    UmiGtk4WorkspaceLayoutHostSnapshot snapshot;
    GtkWidget *root = NULL;
    GtkWidget *button;
    uint64_t revision;
    UmiStatus status;
    int failed = 0;

    layout = calloc(1U, sizeof(*layout));
    REQUIRE(layout != NULL);
    *layout = *source;
    REQUIRE(umi_ui_workspace_layout_set_locked(layout, false) == UMI_STATUS_OK);
    REQUIRE(add_tool_window(layout, "docked", "Navigation", "left", true) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_locked(layout, true) == UMI_STATUS_OK);
    revision = layout->revision;
    if (mode == 0U) {
        status = umi_gtk4_workspace_layout_host_create(layout, create_tool_body, &fixture, &host);
    } else if (mode == 1U) {
        status = umi_gtk4_workspace_layout_host_create_interactive(layout, create_tool_body,
            &fixture, record_tool_action, &fixture, &host);
    } else {
        const UmiGtk4WorkspaceLayoutHostOptions options = { .tool_presentation_actions = 1 };
        status = umi_gtk4_workspace_layout_host_create_with_options(layout, create_tool_body,
            &fixture, NULL, NULL, &options, &host);
    }
    REQUIRE(status == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_workspace_layout_host_widget(host));
    button = find_tool_control(root, "docked.action.close");
    REQUIRE(button == NULL || !gtk_widget_get_sensitive(button));
    button = find_tool_control(root, "docked.action.auto-hide");
    REQUIRE(button == NULL || !gtk_widget_get_sensitive(button));
    button = find_tool_control(root, "docked.tab.close");
    REQUIRE(button == NULL || !gtk_widget_get_sensitive(button));
    REQUIRE(umi_gtk4_workspace_layout_host_reveal_tool_window(host, "structure") == UMI_STATUS_OK);
    REQUIRE(visible_in_host(fixture.structure, root));
    REQUIRE(visible_in_host(fixture.editor, root));
    button = find_tool_control(root, "workstation.tool-dock.left");
    REQUIRE(GTK_IS_BUTTON(button) && !gtk_widget_get_sensitive(button));
    /* Direct signal emission deliberately bypasses GTK's disabled-button
     * input filtering. The semantic handler must still refuse the action. */
    g_signal_emit_by_name(button, "clicked");
    drain_tool_callbacks();
    REQUIRE(fixture.actions == 0U);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.revealed_tool_count == 1U && snapshot.source_layout_revision == revision);
    button = find_tool_control(root, "workstation.tool-close.left");
    REQUIRE(GTK_IS_BUTTON(button) && gtk_widget_get_sensitive(button));
    g_signal_emit_by_name(button, "clicked");
    drain_tool_callbacks();
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.revealed_tool_count == 0U && snapshot.tool_rail_count == 2U);
    REQUIRE(snapshot.source_layout_revision == revision && layout->revision == revision);
    REQUIRE(fixture.actions == 0U && visible_in_host(fixture.editor, root));

cleanup:
    umi_gtk4_workspace_layout_host_destroy(host);
    g_clear_object(&root);
    if (!failed && fixture.created != fixture.released) {
        (void)fprintf(stderr, "Legacy tool host provider reference leak\n");
        failed = 1;
    }
    free(layout);
    return failed;
}

/* Create only the Framework's real host and test providers. A missing display
 * skips the native case rather than reporting its UI assertions as passed. */
int main(void)
{
    ToolWindowFixture fixture = {0};
    UmiUiWorkspaceLayout *layout = NULL;
    UmiUiWorkspaceLayout *blank = NULL;
    UmiUiWorkspaceLayout *original = NULL;
    UmiGtk4WorkspaceLayoutHost *host = NULL;
    UmiGtk4WorkspaceLayoutHostSnapshot snapshot;
    GtkWidget *root = NULL;
    GtkWidget *old_tab = NULL;
    GtkWidget *last_tab = NULL;
    GtkWidget *last_dock = NULL;
    GtkWidget *button;
    GtkWidget *title;
    GtkWidget *editor;
    GtkWidget *structure;
    GtkWidget *overlay;
    GtkWidget *flyout;
    size_t before_created;
    unsigned before_actions;
    int failed = 0;

    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    layout = calloc(1U, sizeof(*layout));
    blank = calloc(1U, sizeof(*blank));
    original = calloc(1U, sizeof(*original));
    REQUIRE(layout != NULL && blank != NULL && original != NULL);
    REQUIRE(umi_ui_workspace_layout_init(blank, "test.empty-tools", "Empty") == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_init(layout, "test.edge-tools", "Edge tools") == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_locked(layout, false) == UMI_STATUS_OK);
    REQUIRE(add_tool_window(layout, "editor", "Editor", "centre", true) == UMI_STATUS_OK);
    REQUIRE(add_tool_window(layout, "structure", "Structure", "auto-hide:left", false) == UMI_STATUS_OK);
    REQUIRE(add_tool_window(layout, "inspector", "Object Inspector", "auto-hide:left", false) == UMI_STATUS_OK);
    REQUIRE(add_tool_window(layout, "ordinary-hidden", "Hidden tool", "left", false) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_locked(layout, true) == UMI_STATUS_OK);
    *original = *layout;
    REQUIRE(verify_legacy_tool_policy(layout, 0U) == 0);
    REQUIRE(verify_legacy_tool_policy(layout, 1U) == 0);
    REQUIRE(verify_legacy_tool_policy(layout, 2U) == 0);
    REQUIRE(umi_gtk4_workspace_layout_host_create_with_options(blank, create_tool_body, &fixture,
        record_tool_action, &fixture,
        &(const UmiGtk4WorkspaceLayoutHostOptions){ .tool_presentation_actions = 1 },
        &host) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_workspace_layout_host_widget(host));
    REQUIRE(umi_gtk4_workspace_layout_host_set_content_retention(host, 1) == UMI_STATUS_OK);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.tool_rail_count == 0U && snapshot.revealed_tool_count == 0U);
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.tool_rail_count == 2U && snapshot.revealed_tool_count == 0U);
    REQUIRE(snapshot.source_layout_revision == original->revision);
    REQUIRE(find_tool_control(root, "workstation.tool-tab.ordinary-hidden") == NULL);
    REQUIRE(umi_gtk4_workspace_layout_host_reveal_tool_window(host, "ordinary-hidden") == UMI_STATUS_NOT_FOUND);
    REQUIRE(umi_gtk4_workspace_layout_host_reveal_tool_window(host, "missing") == UMI_STATUS_NOT_FOUND);
    editor = fixture.editor;
    REQUIRE(GTK_IS_EDITABLE(editor) && visible_in_host(editor, root));
    gtk_editable_set_text(GTK_EDITABLE(editor), "unsaved editor draft");

    /* Reject contradictory or unsupported auto-hide metadata before it can
     * mount one provider in both the centre notebook and an edge drawer. */
    layout->windows[1].visible = true;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_INVALID_ARGUMENT);
    layout->windows[1].visible = false;
    layout->windows[1].floating = true;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_INVALID_ARGUMENT);
    layout->windows[1].floating = false;
    (void)g_strlcpy(layout->windows[1].placement_id, "auto-hide:centre",
        sizeof(layout->windows[1].placement_id));
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_INVALID_ARGUMENT);
    (void)g_strlcpy(layout->windows[1].placement_id, "auto-hide:left",
        sizeof(layout->windows[1].placement_id));
    layout->windows[1].title[0] = (char)0xff;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_INVALID_ARGUMENT);
    memcpy(layout->windows[1].title, original->windows[1].title,
        sizeof(layout->windows[1].title));
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.source_layout_revision == original->revision);
    REQUIRE(snapshot.tool_rail_count == 2U && snapshot.revealed_tool_count == 0U);
    REQUIRE(fixture.editor == editor && visible_in_host(editor, root));
    REQUIRE(strcmp(gtk_editable_get_text(GTK_EDITABLE(editor)), "unsaved editor draft") == 0);

    /* Revealing a tool works while the saved layout is locked and leaves the
     * editor visible. The drawer's title and close/dock controls are real widgets. */
    REQUIRE(umi_gtk4_workspace_layout_host_reveal_tool_window(host, "structure") == UMI_STATUS_OK);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.revealed_tool_count == 1U && snapshot.tool_rail_count == 2U);
    title = find_tool_control(root, "workstation.tool-title.left");
    REQUIRE(GTK_IS_LABEL(title) && strcmp(gtk_label_get_text(GTK_LABEL(title)), "Structure") == 0);
    REQUIRE(visible_in_host(fixture.structure, root));
    REQUIRE(visible_in_host(editor, root));
    REQUIRE(find_tool_control(root, "workstation.tool-dock.left") != NULL);
    button = find_tool_control(root, "workstation.tool-close.left");
    REQUIRE(GTK_IS_BUTTON(button) && gtk_widget_get_sensitive(button));
    /* Preferred size must not become a fixed minimum which hides Close or
     * Dock when the host is smaller than the normal 300 by 230 pixel drawer. */
    gtk_widget_allocate(root, 180, 120, -1, NULL);
    overlay = find_tool_control(root, "workstation.tool-overlay");
    flyout = find_tool_control(root, "workstation.tool-flyout.left");
    REQUIRE(tool_widget_fits(overlay, root));
    REQUIRE(tool_widget_fits(find_tool_control(root, "workstation.tool-drawer.left"), overlay));
    REQUIRE(tool_widget_fits(flyout, overlay));
    REQUIRE(tool_widget_fits(gtk_widget_get_first_child(flyout), overlay));
    REQUIRE(tool_widget_fits(button, overlay));
    REQUIRE(tool_widget_fits(find_tool_control(root, "workstation.tool-dock.left"), overlay));
    gtk_widget_allocate(root, 1000, 700, -1, NULL);
    structure = fixture.structure;
    gtk_editable_set_text(GTK_EDITABLE(structure), "unsaved structure filter");
    before_created = fixture.created;
    REQUIRE(umi_gtk4_workspace_layout_host_reveal_tool_window(host, "structure") == UMI_STATUS_OK);
    REQUIRE(fixture.created == before_created && fixture.structure == structure);
    button = find_tool_control(root, "workstation.tool-tab.inspector");
    REQUIRE(GTK_IS_TOGGLE_BUTTON(button));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    drain_tool_callbacks();
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.revealed_tool_count == 1U);
    title = find_tool_control(root, "workstation.tool-title.left");
    REQUIRE(GTK_IS_LABEL(title) && strcmp(gtk_label_get_text(GTK_LABEL(title)), "Object Inspector") == 0);
    REQUIRE(visible_in_host(fixture.inspector, root) && !visible_in_host(structure, root));
    REQUIRE(fixture.editor == editor && visible_in_host(editor, root));
    REQUIRE(strcmp(gtk_editable_get_text(GTK_EDITABLE(editor)), "unsaved editor draft") == 0);
    button = find_tool_control(root, "workstation.tool-close.left");
    REQUIRE(GTK_IS_BUTTON(button));
    g_signal_emit_by_name(button, "clicked");
    drain_tool_callbacks();
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.revealed_tool_count == 0U && snapshot.tool_rail_count == 2U);
    REQUIRE(!visible_in_host(fixture.inspector, root));
    REQUIRE(fixture.actions == 0U && snapshot.source_layout_revision == original->revision);
    REQUIRE(memcmp(layout, original, sizeof(*layout)) == 0);
    REQUIRE(umi_gtk4_workspace_layout_host_reveal_tool_window(host, "structure") == UMI_STATUS_OK);
    REQUIRE(fixture.structure == structure && visible_in_host(structure, root));
    REQUIRE(strcmp(gtk_editable_get_text(GTK_EDITABLE(structure)), "unsaved structure filter") == 0);
    umi_gtk4_workspace_layout_host_collapse_tool_windows(host);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.revealed_tool_count == 0U && visible_in_host(editor, root));

    /* Pinning uses a dedicated owner request, not the protected-panel flag or
     * an implicit geometry mutation inside the native view. */
    REQUIRE(umi_gtk4_workspace_layout_host_reveal_tool_window(host, "inspector") == UMI_STATUS_OK);
    button = find_tool_control(root, "workstation.tool-dock.left");
    REQUIRE(GTK_IS_BUTTON(button));
    g_signal_emit_by_name(button, "clicked");
    drain_tool_callbacks();
    REQUIRE(fixture.actions == 1U && fixture.last_action == UMI_WS_PANEL_ACTION_AUTO_HIDE_TOGGLE);
    REQUIRE(strcmp(fixture.last_window_id, "inspector") == 0);
    REQUIRE(memcmp(layout, original, sizeof(*layout)) == 0);

    /* A queued or retained tab from old chrome cannot reopen a tool after a
     * model rebuild, including a genuinely empty new canvas. */
    button = find_tool_control(root, "workstation.tool-tab.structure");
    REQUIRE(GTK_IS_TOGGLE_BUTTON(button));
    old_tab = g_object_ref(button);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(old_tab), TRUE);
    button = find_tool_control(root, "workstation.tool-dock.left");
    REQUIRE(GTK_IS_BUTTON(button));
    before_actions = fixture.actions;
    g_signal_emit_by_name(button, "clicked");
    REQUIRE(fixture.actions == before_actions);
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(host, blank) == UMI_STATUS_OK);
    before_created = fixture.created;
    drain_tool_callbacks();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(old_tab), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(old_tab), TRUE);
    drain_tool_callbacks();
    snapshot = umi_gtk4_workspace_layout_host_snapshot(host);
    REQUIRE(snapshot.tool_rail_count == 0U && snapshot.revealed_tool_count == 0U);
    REQUIRE(fixture.created == before_created && fixture.actions == before_actions);
    /* Older valid models may lack a display title. The stable ID supplies a
     * readable tab and drawer title instead of silently dropping the tool. */
    layout->windows[2].title[0] = '\0';
    ++layout->revision;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(host, layout) == UMI_STATUS_OK);
    REQUIRE(fixture.editor == editor && strcmp(gtk_editable_get_text(GTK_EDITABLE(editor)),
        "unsaved editor draft") == 0);
    button = find_tool_control(root, "workstation.tool-tab.inspector");
    REQUIRE(GTK_IS_TOGGLE_BUTTON(button));
    last_tab = g_object_ref(button);
    REQUIRE(strcmp(gtk_widget_get_tooltip_text(last_tab), "inspector") == 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(last_tab), TRUE);
    title = find_tool_control(root, "workstation.tool-title.left");
    REQUIRE(GTK_IS_LABEL(title) && strcmp(gtk_label_get_text(GTK_LABEL(title)), "inspector") == 0);
    button = find_tool_control(root, "workstation.tool-dock.left");
    REQUIRE(GTK_IS_BUTTON(button));
    last_dock = g_object_ref(button);
    g_signal_emit_by_name(last_dock, "clicked");
    REQUIRE(fixture.actions == before_actions);
    umi_gtk4_workspace_layout_host_destroy(host);
    host = NULL;
    drain_tool_callbacks();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(last_tab), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(last_tab), TRUE);
    g_signal_emit_by_name(last_dock, "clicked");
    drain_tool_callbacks();
    REQUIRE(fixture.actions == before_actions);
    REQUIRE(gtk_widget_get_first_child(root) == NULL);

cleanup:
    umi_gtk4_workspace_layout_host_destroy(host);
    g_clear_object(&old_tab);
    g_clear_object(&last_tab);
    g_clear_object(&last_dock);
    g_clear_object(&root);
    if (!failed && fixture.created != fixture.released) {
        (void)fprintf(stderr, "Tool drawer provider reference leak\n");
        failed = 1;
    }
    free(original);
    free(blank);
    free(layout);
    return failed;
}
