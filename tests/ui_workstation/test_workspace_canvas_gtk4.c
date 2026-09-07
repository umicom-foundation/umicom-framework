/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_workspace_canvas_gtk4.c
 *
 * PURPOSE:
 *   Check native in-canvas panels, normalized placement and deferred model
 *   requests without presenting a window or starting another application.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"
#include "umicom/ui/workbench_canvas.h"
#include "umicom/ui/layout_persistence.h"
#include "umicom/application/suite_layout/gtk4_workstation.h"
#include "umicom/application/suite_layout/projection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Keep every check active in release builds and release owned test objects. */
#define REQUIRE(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; \
        goto cleanup; \
    } \
} while (0)

/* The fake owner can record a request, rebuild its host or close it while the
 * request callback is running. No product service or process is involved. */
typedef enum CanvasOwnerAction {
    CANVAS_OWNER_RECORD,
    CANVAS_OWNER_REBUILD,
    CANVAS_OWNER_DESTROY
} CanvasOwnerAction;

typedef struct CanvasFixture {
    UmiGtk4WorkspaceLayoutHost *host;
    UmiUiWorkspaceLayout *layout;
    size_t created_content;
    size_t released_content;
    size_t requests;
    size_t action_requests;
    UmiWsPanelAction last_action;
    char requested_window[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UmiApplicationSuiteLayoutRect requested_rect;
    uint64_t expected_revision;
    CanvasOwnerAction action;
    UmiStatus response;
    UmiStatus callback_status;
} CanvasFixture;

/* Count final disposal, not removal from a parent. A retained widget may stay
 * alive after a rebuild, but must eventually release its provider content. */
static void on_content_released(gpointer user_data, GObject *object)
{
    CanvasFixture *fixture = user_data;
    (void)object;
    fixture->released_content += 1U;
}

/* An intentionally oversized provider exposes accidental minimum-size
 * propagation. The canvas must clip it instead of enlarging the main window. */
static GtkWidget *create_test_panel(const UmiUiWorkspaceWindow *window, void *user_data)
{
    CanvasFixture *fixture = user_data;
    GtkWidget *content = gtk_label_new(window->title);
    gtk_widget_set_size_request(content, 3000, 2000);
    g_object_weak_ref(G_OBJECT(content), on_content_released, fixture);
    fixture->created_content += 1U;
    return content;
}

/* Record copied request values before optional teardown. The owner alone
 * updates the portable layout, then asks GTK to render the new revision. */
static UmiStatus accept_geometry(const char *window_id,
    const UmiApplicationSuiteLayoutRect *rect, uint64_t expected_revision,
    void *user_data)
{
    CanvasFixture *fixture = user_data;
    fixture->requests += 1U;
    (void)g_strlcpy(fixture->requested_window, window_id,
        sizeof(fixture->requested_window));
    fixture->requested_rect = *rect;
    fixture->expected_revision = expected_revision;
    fixture->callback_status = fixture->response;
    if (fixture->response != UMI_STATUS_OK) return fixture->response;
    if (fixture->action == CANVAS_OWNER_DESTROY) {
        umi_gtk4_workspace_layout_host_destroy(fixture->host);
        fixture->host = NULL;
    } else if (fixture->action == CANVAS_OWNER_REBUILD) {
        fixture->callback_status = umi_ui_workspace_layout_place_window(
            fixture->layout, window_id, rect->x, rect->y, rect->width, rect->height);
        if (fixture->callback_status == UMI_STATUS_OK) {
            fixture->callback_status = umi_gtk4_workspace_layout_host_rebuild(
                fixture->host, fixture->layout);
        }
    }
    return fixture->callback_status;
}

/* Record semantic frame/tab actions without opening menus or product tools. */
static void record_panel_action(const char *window_id, UmiWsPanelAction action,
    void *user_data)
{
    CanvasFixture *fixture = user_data;
    (void)window_id;
    fixture->action_requests += 1U;
    fixture->last_action = action;
}

/* Match stable automation tags rather than translated captions or child order. */
static GtkWidget *find_tag(GtkWidget *widget, const char *tag)
{
    GtkWidget *child;
    const char *actual;
    if (widget == NULL) return NULL;
    actual = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (actual != NULL && strcmp(actual, tag) == 0) return widget;
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_tag(child, tag);
        if (found != NULL) return found;
    }
    return NULL;
}

/* A pure canvas must not silently turn internal windows into notebook pages. */
static bool contains_notebook(GtkWidget *widget)
{
    GtkWidget *child;
    if (GTK_IS_NOTEBOOK(widget)) return true;
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        if (contains_notebook(child)) return true;
    }
    return false;
}

/* Populate a small valid model through the public layout API. Keeping its
 * sizeable fixed-capacity window collection on the heap avoids stack pressure. */
static UmiStatus add_canvas_panel(UmiUiWorkspaceLayout *layout, const char *id,
    UmiApplicationSuiteLayoutRect rect, bool visible)
{
    UmiUiWorkspaceWindow window = {0};
    if (g_strlcpy(window.window_id, id, sizeof(window.window_id)) >= sizeof(window.window_id) ||
        g_strlcpy(window.title, id, sizeof(window.title)) >= sizeof(window.title) ||
        g_strlcpy(window.stack_id, id, sizeof(window.stack_id)) >= sizeof(window.stack_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)g_strlcpy(window.tool_id, "canvas.test.provider", sizeof(window.tool_id));
    (void)g_strlcpy(window.placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT,
        sizeof(window.placement_id));
    window.x = rect.x;
    window.y = rect.y;
    window.width = rect.width;
    window.height = rect.height;
    window.visible = visible;
    window.closable = true;
    window.resizable = true;
    window.z_order = (int32_t)layout->window_count;
    return umi_ui_workspace_layout_add_window(layout, &window);
}

/* Allow one pixel of rounding when a normalized fraction becomes a physical
 * allocation. Comparison does not depend on screenshots or a mapped window. */
static bool near_pixel(double actual, double expected)
{
    return actual >= expected - 1.0 && actual <= expected + 1.0;
}

/* Compare all four sides relative to the canvas, not the provider's large
 * requested size or an unrelated top-level screen coordinate. */
static bool panel_has_bounds(GtkWidget *panel, GtkWidget *canvas,
    double x, double y, double width, double height)
{
    graphene_rect_t bounds;
    if (panel == NULL || canvas == NULL) return false;
    if (!gtk_widget_compute_bounds(panel, canvas, &bounds)) return false;
    return near_pixel(bounds.origin.x, x) && near_pixel(bounds.origin.y, y) &&
        near_pixel(bounds.size.width, width) && near_pixel(bounds.size.height, height);
}

/* Service only ready main-context work. A bounded, nonblocking loop cannot
 * hang waiting for a user event and never presents an application window. */
static void drain_ready_callbacks(void)
{
    unsigned iteration;
    for (iteration = 0U; iteration < 256U; ++iteration) {
        if (!g_main_context_iteration(NULL, FALSE)) break;
    }
}

/* Release the owned controller-list model and return one owned drag-controller
 * reference so tests can send signals without depending on controller order. */
static GtkGestureDrag *find_drag_controller(GtkWidget *widget)
{
    GListModel *controllers;
    GtkGestureDrag *found = NULL;
    guint index;
    if (widget == NULL) return NULL;
    controllers = gtk_widget_observe_controllers(widget);
    for (index = 0U; index < g_list_model_get_n_items(controllers); ++index) {
        GObject *controller = g_list_model_get_item(controllers, index);
        if (GTK_IS_GESTURE_DRAG(controller)) {
            found = GTK_GESTURE_DRAG(controller);
            break;
        }
        g_object_unref(controller);
    }
    g_object_unref(controllers);
    return found;
}

/* Retained gesture widgets must not reach a disposed controller. These
 * synthetic signals do not move the mouse or send desktop input events. */
static void emit_retained_drag(GtkWidget *widget)
{
    GListModel *controllers = gtk_widget_observe_controllers(widget);
    guint index;
    for (index = 0U; index < g_list_model_get_n_items(controllers); ++index) {
        GObject *controller = g_list_model_get_item(controllers, index);
        if (GTK_IS_GESTURE_DRAG(controller)) {
            g_signal_emit_by_name(controller, "drag-begin", 8.0, 8.0);
            g_signal_emit_by_name(controller, "drag-update", 24.0, 16.0);
            g_signal_emit_by_name(controller, "drag-end", 24.0, 16.0);
        }
        g_object_unref(controller);
    }
    g_object_unref(controllers);
}

/* Find the readable name supplied by this test in the real layout dropdown.
 * This proves custom layouts reach the GUI model, not just a private counter. */
static guint find_layout_choice(GtkDropDown *dropdown, const char *name)
{
    GListModel *model = gtk_drop_down_get_model(dropdown);
    guint index;
    if (model == NULL) return GTK_INVALID_LIST_POSITION;
    for (index = 0U; index < g_list_model_get_n_items(model); ++index) {
        GtkStringObject *choice = g_list_model_get_item(model, index);
        bool matches = GTK_IS_STRING_OBJECT(choice) &&
            strcmp(gtk_string_object_get_string(choice), name) == 0;
        if (choice != NULL) g_object_unref(choice);
        if (matches) return index;
    }
    return GTK_INVALID_LIST_POSITION;
}

/* Exercise the shared suite shell using each product's real panel catalogue.
 * Studio here means its Framework experience, not its separate IDE window.
 * The injected provider supplies labels only; no business service is started. */
static int verify_suite_canvas(const char *application_id, const char *panel_id)
{
    CanvasFixture provider = {0};
    UmiApplicationSuiteGtk4Workstation *workstation = NULL;
    UmiApplicationSuiteGtk4WorkstationConfig config = {0};
    UmiApplicationSuiteGtk4WorkstationSnapshot snapshot;
    UmiApplicationSuiteGtk4WorkstationSnapshot before_import;
    UmiUiWorkspaceLayout *default_layout = NULL;
    UmiUiWorkspacePanelSettings settings;
    UmiUiWorkspaceCanvasClearResult clear_result;
    UmiUiWorkspaceImportReport import_report;
    char *exported_layout = NULL;
    GtkWidget *root;
    GtkWidget *dropdown;
    GtkWidget *canvas;
    GtkWidget *panel;
    char custom_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char missing_suffix[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char window_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    char panel_tag[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY + 40U];
    size_t initial_layout_count;
    size_t index;
    guint choice;
    int written;
    int failed = 0;

    default_layout = calloc(1U, sizeof(*default_layout));
    REQUIRE(default_layout != NULL);
    REQUIRE(umi_application_suite_layout_project_default(application_id, default_layout) ==
        UMI_STATUS_OK);
    /* Fail before GTK construction if a future default introduces detached
     * windows: this regression must remain an unpresented-widget test. */
    for (index = 0U; index < default_layout->window_count; ++index) {
        REQUIRE(!default_layout->windows[index].visible || !default_layout->windows[index].floating);
    }
    written = g_snprintf(custom_id, sizeof(custom_id), "%s.canvas-test", application_id);
    REQUIRE(written >= 0 && (size_t)written < sizeof(custom_id));
    config.application_id = application_id;
    config.title = "Canvas acceptance";
    config.mode_badge = "Test";
    config.panel_factory = create_test_panel;
    config.user_data = &provider;
    REQUIRE(umi_application_suite_gtk4_workstation_create(&config, &workstation) == UMI_STATUS_OK);
    root = umi_application_suite_gtk4_workstation_widget(workstation);
    REQUIRE(root != NULL);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    initial_layout_count = snapshot.layout_count;
    REQUIRE(umi_application_suite_gtk4_workstation_create_blank_layout(workstation,
        "unqualified-layout", "Rejected layout") == UMI_STATUS_INVALID_ARGUMENT);
    /* A product prefix alone does not name a layout. Reject it without adding
     * an unusable item to the selector or changing the current arrangement. */
    written = g_snprintf(missing_suffix, sizeof(missing_suffix), "%s.", application_id);
    REQUIRE(written >= 0 && (size_t)written < sizeof(missing_suffix));
    REQUIRE(umi_application_suite_gtk4_workstation_create_blank_layout(workstation,
        missing_suffix, "Missing layout name") == UMI_STATUS_INVALID_ARGUMENT);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(snapshot.layout_count == initial_layout_count);
    REQUIRE(umi_application_suite_gtk4_workstation_create_blank_layout(workstation,
        custom_id, "Acceptance canvas") == UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(snapshot.layout_count == initial_layout_count + 1U);
    REQUIRE(snapshot.layout_locked && !snapshot.editing_layout);
    REQUIRE(snapshot.rendered_panel_count == 0U && snapshot.canvas_panel_count == 0U);
    REQUIRE(strcmp(snapshot.active_layout_id, custom_id) == 0);
    dropdown = find_tag(root, "umicom.layout.selector");
    REQUIRE(GTK_IS_DROP_DOWN(dropdown));
    REQUIRE(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(dropdown))) ==
        snapshot.layout_count);
    choice = find_layout_choice(GTK_DROP_DOWN(dropdown), "Acceptance canvas");
    REQUIRE(choice != GTK_INVALID_LIST_POSITION);

    /* Cancel restores the empty baseline without deleting the named layout. */
    REQUIRE(umi_application_suite_gtk4_workstation_begin_layout_edit(workstation) == UMI_STATUS_OK);
    REQUIRE(umi_application_suite_gtk4_workstation_open_window(workstation, panel_id,
        UMI_UI_WORKSPACE_CANVAS_PLACEMENT, 0, 1U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    settings = umi_ui_workspace_panel_settings_default(window_id);
    settings.placement_id = UMI_UI_WORKSPACE_CANVAS_PLACEMENT;
    settings.stack_id = window_id;
    settings.x = 0.15;
    settings.y = 0.125;
    settings.width = 0.45;
    settings.height = 0.50;
    REQUIRE(umi_application_suite_gtk4_workstation_apply_panel_settings(workstation, &settings) ==
        UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(snapshot.canvas_panel_count == 1U && snapshot.editing_layout);
    REQUIRE(umi_application_suite_gtk4_workstation_cancel_layout_edit(workstation) == UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(snapshot.canvas_panel_count == 0U && snapshot.rendered_panel_count == 0U);
    REQUIRE(snapshot.layout_locked && strcmp(snapshot.active_layout_id, custom_id) == 0);

    /* Commit the same real tool and inspect its actual native rectangle. */
    REQUIRE(umi_application_suite_gtk4_workstation_begin_layout_edit(workstation) == UMI_STATUS_OK);
    REQUIRE(umi_application_suite_gtk4_workstation_open_window(workstation, panel_id,
        UMI_UI_WORKSPACE_CANVAS_PLACEMENT, 0, 2U, window_id, sizeof(window_id)) == UMI_STATUS_OK);
    REQUIRE(umi_application_suite_gtk4_workstation_apply_panel_settings(workstation, &settings) ==
        UMI_STATUS_OK);
    REQUIRE(umi_application_suite_gtk4_workstation_commit_layout_edit(workstation) == UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(snapshot.canvas_panel_count == 1U && snapshot.layout_locked && !snapshot.editing_layout);
    written = g_snprintf(panel_tag, sizeof(panel_tag), "workstation.canvas.panel.%s", window_id);
    REQUIRE(written >= 0 && (size_t)written < sizeof(panel_tag));
    canvas = find_tag(root, "workstation.workspace-canvas");
    panel = find_tag(root, panel_tag);
    REQUIRE(canvas != NULL && panel != NULL);
    gtk_widget_allocate(canvas, 1000, 800, -1, NULL);
    REQUIRE(panel_has_bounds(panel, canvas, 150.0, 100.0, 450.0, 400.0));

    /* Choosing the custom entry through GTK must work after leaving it for a
     * product default; the immutable product selector cannot lose that entry. */
    REQUIRE(umi_application_suite_gtk4_workstation_select_layout(workstation,
        default_layout->layout_id) == UMI_STATUS_OK);
    choice = find_layout_choice(GTK_DROP_DOWN(dropdown), "Acceptance canvas");
    REQUIRE(choice != GTK_INVALID_LIST_POSITION);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), choice);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(strcmp(snapshot.active_layout_id, custom_id) == 0 && snapshot.canvas_panel_count == 1U);
    REQUIRE(umi_application_suite_gtk4_workstation_begin_layout_edit(workstation) == UMI_STATUS_OK);
    REQUIRE(umi_application_suite_gtk4_workstation_clear_canvas(workstation, &clear_result) ==
        UMI_STATUS_OK);
    REQUIRE(clear_result.removed == 1U && clear_result.retained == 0U);
    REQUIRE(umi_application_suite_gtk4_workstation_cancel_layout_edit(workstation) == UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(snapshot.canvas_panel_count == 1U);
    REQUIRE(umi_application_suite_gtk4_workstation_begin_layout_edit(workstation) == UMI_STATUS_OK);
    REQUIRE(umi_application_suite_gtk4_workstation_set_window_pinned(workstation, window_id, 1) ==
        UMI_STATUS_OK);
    REQUIRE(umi_application_suite_gtk4_workstation_clear_canvas(workstation, &clear_result) ==
        UMI_STATUS_OK);
    REQUIRE(clear_result.removed == 0U && clear_result.retained == 1U);
    REQUIRE(umi_application_suite_gtk4_workstation_cancel_layout_edit(workstation) == UMI_STATUS_OK);

    /* Export to memory, then restore into a fresh workstation that has never
     * seen this custom layout. Only one heavyweight suite host stays alive at
     * a time, and no files or persistent user preferences are written. */
    exported_layout = calloc(UMI_UI_LAYOUT_ENCODED_CAPACITY, 1U);
    REQUIRE(exported_layout != NULL);
    REQUIRE(umi_application_suite_gtk4_workstation_export_layout(workstation, 5U,
        exported_layout, UMI_UI_LAYOUT_ENCODED_CAPACITY) == UMI_STATUS_OK);
    umi_application_suite_gtk4_workstation_destroy(workstation);
    workstation = NULL;
    REQUIRE(provider.created_content == provider.released_content);
    REQUIRE(umi_application_suite_gtk4_workstation_create(&config, &workstation) == UMI_STATUS_OK);
    REQUIRE(umi_application_suite_gtk4_workstation_import_layout(workstation,
        exported_layout, 1, &import_report) == UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(strcmp(snapshot.active_layout_id, custom_id) == 0);
    REQUIRE(snapshot.canvas_panel_count == 1U && snapshot.layout_locked);
    root = umi_application_suite_gtk4_workstation_widget(workstation);
    canvas = find_tag(root, "workstation.workspace-canvas");
    panel = find_tag(root, panel_tag);
    REQUIRE(canvas != NULL && panel != NULL);
    gtk_widget_allocate(canvas, 1000, 800, -1, NULL);
    REQUIRE(panel_has_bounds(panel, canvas, 150.0, 100.0, 450.0, 400.0));
    umi_application_suite_gtk4_workstation_destroy(workstation);
    workstation = NULL;
    REQUIRE(provider.created_content == provider.released_content);

    /* A different application must not import another product's qualified
     * canvas and thereby bypass its panel permissions. Missing dependencies
     * may reject it before the prefix check, so require failure and unchanged
     * observable state rather than one error code for every foreign layout. */
    config.application_id = strcmp(application_id, "org.umicom.bank") == 0
        ? "org.umicom.trader" : "org.umicom.bank";
    REQUIRE(umi_application_suite_layout_project_default(config.application_id, default_layout) ==
        UMI_STATUS_OK);
    for (index = 0U; index < default_layout->window_count; ++index) {
        REQUIRE(!default_layout->windows[index].visible || !default_layout->windows[index].floating);
    }
    REQUIRE(umi_application_suite_gtk4_workstation_create(&config, &workstation) == UMI_STATUS_OK);
    before_import = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(umi_application_suite_gtk4_workstation_import_layout(workstation,
        exported_layout, 1, &import_report) != UMI_STATUS_OK);
    snapshot = umi_application_suite_gtk4_workstation_snapshot(workstation);
    REQUIRE(strcmp(snapshot.active_layout_id, before_import.active_layout_id) == 0);
    REQUIRE(snapshot.layout_count == before_import.layout_count);
    REQUIRE(snapshot.rendered_panel_count == before_import.rendered_panel_count);
    REQUIRE(snapshot.canvas_panel_count == before_import.canvas_panel_count);
    REQUIRE(snapshot.source_layout_revision == before_import.source_layout_revision);
    REQUIRE(snapshot.revision == before_import.revision);

cleanup:
    umi_application_suite_gtk4_workstation_destroy(workstation);
    if (provider.created_content != provider.released_content) {
        (void)fprintf(stderr, "Suite provider content was not released: %s\n", application_id);
        failed = 1;
    }
    free(exported_layout);
    free(default_layout);
    return failed;
}

/* Find one owned key/focus controller attached to the title only. */
static GObject *find_canvas_controller(GtkWidget *widget, GType type)
{
    GListModel *controllers = gtk_widget_observe_controllers(widget);
    GObject *found = NULL;
    guint index;
    for (index = 0U; index < g_list_model_get_n_items(controllers); ++index) {
        GObject *candidate = g_list_model_get_item(controllers, index);
        if (G_TYPE_CHECK_INSTANCE_TYPE(candidate, type)) { found = candidate; break; }
        g_object_unref(candidate);
    }
    g_object_unref(controllers);
    return found;
}

/* Synthetic controller signals never send keys to the user's desktop. */
static gboolean emit_canvas_key(GObject *controller, guint key, GdkModifierType state)
{
    gboolean handled = FALSE;
    g_signal_emit_by_name(controller, "key-pressed", key, 0U, state, &handled);
    return handled;
}

/* Small tolerance checks normalized geometry without screen-pixel rounding. */
static bool canvas_near(double actual, double expected)
{
    return actual >= expected - 0.000000001 && actual <= expected + 0.000000001;
}

/* Exercise every actual handle and the keyboard preview/accept/cancel path.
 * The owner records values only, so rejected or uncommitted previews must
 * always return to the same original rectangle without replacing its body. */
static int verify_canvas_edit_controls(void)
{
    static const struct {
        const char *suffix;
        int left, top, right, bottom;
    } edges[] = {
        { "", 0, 0, 1, 1 }, { "north.", 0, 1, 0, 0 },
        { "north-east.", 0, 1, 1, 0 }, { "east.", 0, 0, 1, 0 },
        { "south.", 0, 0, 0, 1 }, { "south-west.", 1, 0, 0, 1 },
        { "west.", 1, 0, 0, 0 }, { "north-west.", 1, 1, 0, 0 }
    };
    CanvasFixture fixture = {0};
    UmiApplicationSuiteLayoutRect original = { 0.125, 0.125, 0.50, 0.50 };
    UmiApplicationSuiteLayoutRect observed;
    GtkWidget *root = NULL;
    GtkWidget *canvas;
    GtkWidget *panel;
    GtkWidget *title;
    GtkGestureDrag *gesture = NULL;
    GtkGestureDrag *other_gesture = NULL;
    GObject *keys = NULL;
    GObject *focus = NULL;
    size_t index;
    size_t before;
    char tag[96];
    int failed = 0;

    fixture.layout = calloc(1U, sizeof(*fixture.layout));
    REQUIRE(fixture.layout != NULL);
    REQUIRE(umi_ui_workspace_layout_init(fixture.layout, "test.controls", "Canvas controls") == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_locked(fixture.layout, false) == UMI_STATUS_OK);
    REQUIRE(add_canvas_panel(fixture.layout, "alpha", original, true) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_create_interactive(fixture.layout, create_test_panel,
        &fixture, record_panel_action, &fixture, &fixture.host) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(
        fixture.host, accept_geometry, &fixture) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_workspace_layout_host_widget(fixture.host));
    canvas = find_tag(root, "workstation.workspace-canvas");
    panel = find_tag(root, "workstation.canvas.panel.alpha");
    title = find_tag(root, "workstation.canvas.drag.alpha");
    REQUIRE(canvas != NULL && panel != NULL && title != NULL);
    REQUIRE(gtk_widget_get_focusable(title));
    gtk_widget_allocate(canvas, 1000, 800, -1, NULL);
    for (index = 0U; index < sizeof(edges) / sizeof(edges[0]); ++index) {
        GtkWidget *handle;
        graphene_rect_t bounds;
        int written = snprintf(tag, sizeof(tag), "workstation.canvas.resize.%salpha", edges[index].suffix);
        REQUIRE(written >= 0 && (size_t)written < sizeof(tag));
        handle = find_tag(root, tag);
        REQUIRE(handle != NULL && gtk_widget_get_visible(handle) && gtk_widget_get_sensitive(handle));
        REQUIRE(gtk_widget_compute_bounds(handle, panel, &bounds));
        REQUIRE(bounds.origin.x >= 0.0F && bounds.origin.y >= 0.0F);
        REQUIRE(bounds.size.width > 0.0F && bounds.size.height > 0.0F);
        REQUIRE(bounds.origin.x + bounds.size.width <= 500.0F);
        REQUIRE(bounds.origin.y + bounds.size.height <= 400.0F);
        /* The square corner is a visual allocation, not a square hit target.
         * Interior chrome must remain reachable through its L-shaped border. */
        REQUIRE(gtk_widget_contains(handle, bounds.size.width / 2.0, bounds.size.height / 2.0) ==
            !((edges[index].left || edges[index].right) && (edges[index].top || edges[index].bottom)));
        REQUIRE(gtk_widget_contains(handle,
            edges[index].left ? 1.0 : bounds.size.width - 1.0,
            edges[index].top ? 1.0 : bounds.size.height - 1.0));
        {
            GtkWidget *close_button = find_tag(root, "alpha.action.close");
            graphene_point_t button_point;
            graphene_point_t grip_point;
            REQUIRE(GTK_IS_BUTTON(close_button));
            button_point = GRAPHENE_POINT_INIT(
                (float)gtk_widget_get_width(close_button) / 2.0F,
                (float)gtk_widget_get_height(close_button) / 2.0F);
            REQUIRE(gtk_widget_compute_point(close_button, handle, &button_point, &grip_point));
            REQUIRE(!gtk_widget_contains(handle, grip_point.x, grip_point.y));
        }
        gesture = find_drag_controller(handle);
        REQUIRE(gesture != NULL);
        g_signal_emit_by_name(gesture, "drag-begin", 1.0, 1.0);
        g_signal_emit_by_name(gesture, "drag-update", 50.0, 40.0);
        REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha", &observed) == UMI_STATUS_OK);
        REQUIRE(canvas_near(observed.x, original.x + (edges[index].left ? 0.05 : 0.0)));
        REQUIRE(canvas_near(observed.y, original.y + (edges[index].top ? 0.05 : 0.0)));
        REQUIRE(canvas_near(observed.width, original.width + (edges[index].right - edges[index].left) * 0.05));
        REQUIRE(canvas_near(observed.height, original.height + (edges[index].bottom - edges[index].top) * 0.05));
        REQUIRE(fixture.requests == 0U);
        g_signal_emit_by_name(gesture, "cancel", NULL);
        g_clear_object(&gesture);
    }
    /* A second handle cannot update or end the first handle's active drag. */
    gesture = find_drag_controller(find_tag(root, "workstation.canvas.resize.alpha"));
    other_gesture = find_drag_controller(find_tag(root, "workstation.canvas.resize.north-west.alpha"));
    REQUIRE(gesture != NULL && other_gesture != NULL);
    g_signal_emit_by_name(gesture, "drag-begin", 1.0, 1.0);
    g_signal_emit_by_name(other_gesture, "drag-update", 50.0, 40.0);
    g_signal_emit_by_name(other_gesture, "drag-end", 50.0, 40.0);
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha", &observed) == UMI_STATUS_OK);
    REQUIRE(canvas_near(observed.x, original.x) && canvas_near(observed.width, original.width));
    g_signal_emit_by_name(gesture, "cancel", NULL);
    g_clear_object(&other_gesture);

    keys = find_canvas_controller(title, GTK_TYPE_EVENT_CONTROLLER_KEY);
    focus = find_canvas_controller(title, GTK_TYPE_EVENT_CONTROLLER_FOCUS);
    REQUIRE(keys != NULL && focus != NULL);
    REQUIRE(!emit_canvas_key(keys, GDK_KEY_Right, GDK_CONTROL_MASK));
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Right, 0));
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Down, GDK_SHIFT_MASK));
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha", &observed) == UMI_STATUS_OK);
    REQUIRE(canvas_near(observed.x, 0.15) && canvas_near(observed.y, 0.125));
    REQUIRE(canvas_near(observed.width, 0.50) && canvas_near(observed.height, 0.525));
    REQUIRE(fixture.requests == 0U && gtk_widget_has_css_class(panel, "canvas-keyboard-edit"));
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Escape, 0));
    REQUIRE(!gtk_widget_has_css_class(panel, "canvas-keyboard-edit"));
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Left, 0));
    g_signal_emit_by_name(focus, "leave");
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha", &observed) == UMI_STATUS_OK);
    REQUIRE(canvas_near(observed.x, original.x) && fixture.requests == 0U);
    REQUIRE(!emit_canvas_key(keys, GDK_KEY_Return, 0));
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Right, 0));
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Return, 0));
    REQUIRE(fixture.requests == 0U);
    REQUIRE(umi_gtk4_workspace_layout_host_snapshot(fixture.host).geometry_pending);
    drain_ready_callbacks();
    REQUIRE(fixture.requests == 1U && fixture.expected_revision == fixture.layout->revision);
    REQUIRE(canvas_near(fixture.requested_rect.x, 0.15));
    REQUIRE(fixture.created_content == 1U);

    /* Rebuild cancels an active preview and disconnects retained old keys and
     * edge controllers. Locked and pinned titles cannot start a new request. */
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Down, 0));
    REQUIRE(umi_ui_workspace_layout_set_locked(fixture.layout, true) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    REQUIRE(!emit_canvas_key(keys, GDK_KEY_Return, 0));
    g_signal_emit_by_name(gesture, "drag-begin", 1.0, 1.0);
    g_signal_emit_by_name(gesture, "drag-end", 50.0, 40.0);
    g_clear_object(&keys);
    g_clear_object(&focus);
    g_clear_object(&gesture);
    title = find_tag(root, "workstation.canvas.drag.alpha");
    REQUIRE(!gtk_widget_get_focusable(title));
    keys = find_canvas_controller(title, GTK_TYPE_EVENT_CONTROLLER_KEY);
    REQUIRE(keys != NULL && !emit_canvas_key(keys, GDK_KEY_Right, 0));
    REQUIRE(umi_ui_workspace_layout_set_locked(fixture.layout, false) == UMI_STATUS_OK);
    fixture.layout->windows[0].pinned = true;
    fixture.layout->revision += 1U;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    g_clear_object(&keys);
    title = find_tag(root, "workstation.canvas.drag.alpha");
    keys = find_canvas_controller(title, GTK_TYPE_EVENT_CONTROLLER_KEY);
    REQUIRE(keys != NULL && !emit_canvas_key(keys, GDK_KEY_Right, 0));
    fixture.layout->windows[0].pinned = false;
    fixture.layout->windows[0].resizable = false;
    fixture.layout->revision += 1U;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    g_clear_object(&keys);
    title = find_tag(root, "workstation.canvas.drag.alpha");
    keys = find_canvas_controller(title, GTK_TYPE_EVENT_CONTROLLER_KEY);
    REQUIRE(keys != NULL && !emit_canvas_key(keys, GDK_KEY_Right, GDK_SHIFT_MASK));
    canvas = find_tag(root, "workstation.workspace-canvas");
    REQUIRE(canvas != NULL);
    gtk_widget_allocate(canvas, 1000, 800, -1, NULL);
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Right, 0));
    before = fixture.requests;
    REQUIRE(emit_canvas_key(keys, GDK_KEY_Return, 0));
    umi_gtk4_workspace_layout_host_destroy(fixture.host);
    fixture.host = NULL;
    REQUIRE(!emit_canvas_key(keys, GDK_KEY_Return, 0));
    drain_ready_callbacks();
    REQUIRE(fixture.requests == before);
cleanup:
    umi_gtk4_workspace_layout_host_destroy(fixture.host);
    if (keys != NULL) g_object_unref(keys);
    if (focus != NULL) g_object_unref(focus);
    if (gesture != NULL) g_object_unref(gesture);
    if (other_gesture != NULL) g_object_unref(other_gesture);
    if (root != NULL) g_object_unref(root);
    if (fixture.created_content != fixture.released_content) failed = 1;
    free(fixture.layout);
    return failed;
}

/* A display connection is required to construct GTK objects, but no window
 * is realized or presented. Missing display support is a skip, not a pass. */
int main(void)
{
    CanvasFixture fixture = {0};
    UmiUiWorkspaceLayout *empty_layout = NULL;
    UmiGtk4WorkspaceLayoutHostSnapshot snapshot;
    UmiApplicationSuiteLayoutRect original = {0.10, 0.125, 0.40, 0.50};
    UmiApplicationSuiteLayoutRect second = {0.60, 0.25, 0.30, 0.50};
    UmiApplicationSuiteLayoutRect changed = {0.20, 0.20, 0.35, 0.35};
    UmiApplicationSuiteLayoutRect observed;
    UmiApplicationSuiteLayoutRect invalid = {-0.1, 0.2, 0.3, 0.3};
    GtkWidget *root = NULL;
    GtkWidget *canvas;
    GtkWidget *panel;
    GtkWidget *action_button;
    GtkWidget *retained_drag = NULL;
    GtkWidget *retained_frame_action = NULL;
    GtkWidget *retained_tab_action = NULL;
    GtkGestureDrag *gesture = NULL;
    size_t before;
    size_t created;
    uint64_t source_revision;
    int minimum;
    int failed = 0;

    if (!gtk_init_check()) {
        (void)puts("SKIP: GTK display is unavailable.");
        return 77;
    }
    fixture.layout = calloc(1U, sizeof(*fixture.layout));
    empty_layout = calloc(1U, sizeof(*empty_layout));
    REQUIRE(fixture.layout != NULL && empty_layout != NULL);
    REQUIRE(umi_ui_workspace_layout_init(fixture.layout, "test.canvas", "Canvas test") ==
        UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_locked(fixture.layout, false) == UMI_STATUS_OK);
    REQUIRE(add_canvas_panel(fixture.layout, "alpha", original, true) == UMI_STATUS_OK);
    REQUIRE(add_canvas_panel(fixture.layout, "beta", second, true) == UMI_STATUS_OK);
    REQUIRE(add_canvas_panel(fixture.layout, "hidden", second, false) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_create_interactive(fixture.layout, create_test_panel,
        &fixture, record_panel_action, &fixture, &fixture.host) == UMI_STATUS_OK);
    root = umi_gtk4_workspace_layout_host_widget(fixture.host);
    REQUIRE(root != NULL);
    g_object_ref(root);
    source_revision = fixture.layout->revision;
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &changed, source_revision) == UMI_STATUS_UNAVAILABLE);
    REQUIRE(umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(
        fixture.host, accept_geometry, &fixture) == UMI_STATUS_OK);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    REQUIRE(snapshot.canvas_count == 2U && snapshot.panel_count == 2U);
    REQUIRE(snapshot.stack_count == 0U && snapshot.floating_count == 0U);
    REQUIRE(snapshot.canvas_editable && !snapshot.geometry_pending);
    REQUIRE(snapshot.source_layout_revision == source_revision);
    REQUIRE(fixture.created_content == 2U);
    REQUIRE(!contains_notebook(root));
    /* A saved library rename updates future gesture revisions without
     * replacing canvas frames or asking providers to recreate their content. */
    ++fixture.layout->windows[2].z_order;
    REQUIRE(umi_gtk4_workspace_layout_host_update_metadata(fixture.host,
        fixture.layout) == UMI_STATUS_INVALID_STATE);
    --fixture.layout->windows[2].z_order;
    (void)g_strlcpy(fixture.layout->name, "Renamed canvas", sizeof(fixture.layout->name));
    ++fixture.layout->revision;
    REQUIRE(umi_gtk4_workspace_layout_host_update_metadata(fixture.host,
        fixture.layout) == UMI_STATUS_OK);
    source_revision = fixture.layout->revision;
    REQUIRE(umi_gtk4_workspace_layout_host_snapshot(fixture.host).source_layout_revision == source_revision);
    REQUIRE(fixture.created_content == 2U && fixture.released_content == 0U);
    canvas = find_tag(root, "workstation.workspace-canvas");
    panel = find_tag(root, "workstation.canvas.panel.alpha");
    REQUIRE(canvas != NULL && panel != NULL);
    REQUIRE(!GTK_IS_NOTEBOOK(canvas));
    gtk_widget_measure(canvas, GTK_ORIENTATION_HORIZONTAL, -1, &minimum, NULL, NULL, NULL);
    REQUIRE(minimum <= 1000);
    gtk_widget_allocate(canvas, 1000, 800, -1, NULL);
    REQUIRE(panel_has_bounds(panel, canvas, 100.0, 100.0, 400.0, 400.0));
    REQUIRE(panel_has_bounds(find_tag(root, "workstation.canvas.panel.beta"),
        canvas, 600.0, 200.0, 300.0, 400.0));
    gtk_widget_allocate(canvas, 800, 600, -1, NULL);
    REQUIRE(panel_has_bounds(panel, canvas, 80.0, 75.0, 320.0, 300.0));
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha",
        &observed) == UMI_STATUS_OK);
    REQUIRE(observed.x == original.x && observed.width == original.width);
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "hidden",
        &observed) == UMI_STATUS_NOT_FOUND);
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "missing", &changed, source_revision) == UMI_STATUS_NOT_FOUND);
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &invalid, source_revision) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &changed, source_revision + 1U) == UMI_STATUS_INVALID_STATE);

    /* Actual attached gesture callbacks preview a move and a resize without
     * committing. Cancelling restores the model and sends no owner request. */
    gesture = find_drag_controller(find_tag(root, "workstation.canvas.drag.alpha"));
    REQUIRE(gesture != NULL);
    g_signal_emit_by_name(gesture, "drag-begin", 8.0, 8.0);
    g_signal_emit_by_name(gesture, "drag-update", 100.0, 80.0);
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha", &observed) ==
        UMI_STATUS_OK);
    REQUIRE(observed.x > original.x && observed.y > original.y);
    REQUIRE(observed.width == original.width && fixture.requests == 0U);
    REQUIRE(umi_gtk4_workspace_layout_host_update_metadata(fixture.host,
        fixture.layout) == UMI_STATUS_BUSY);
    g_signal_emit_by_name(gesture, "cancel", NULL);
    g_object_unref(gesture);
    gesture = NULL;
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha", &observed) ==
        UMI_STATUS_OK);
    REQUIRE(observed.x == original.x && observed.y == original.y);
    gesture = find_drag_controller(find_tag(root, "workstation.canvas.resize.alpha"));
    REQUIRE(gesture != NULL);
    g_signal_emit_by_name(gesture, "drag-begin", 8.0, 8.0);
    g_signal_emit_by_name(gesture, "drag-update", 100.0, 80.0);
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha", &observed) ==
        UMI_STATUS_OK);
    REQUIRE(observed.width > original.width && observed.height > original.height);
    g_signal_emit_by_name(gesture, "cancel", NULL);
    g_object_unref(gesture);
    gesture = NULL;
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    REQUIRE(!snapshot.geometry_pending && fixture.requests == 0U);

    /* Acceptance belongs to the model owner: queued requests are not a claim
     * that the layout has already changed, and duplicate work returns BUSY. */
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &changed, source_revision) == UMI_STATUS_OK);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    REQUIRE(snapshot.geometry_pending && fixture.requests == 0U);
    REQUIRE(umi_gtk4_workspace_layout_host_update_metadata(fixture.host,
        fixture.layout) == UMI_STATUS_BUSY);
    /* Validation happens before publication or cancellation. A malformed new
     * rectangle must leave the existing view and its queued request intact. */
    memset(fixture.layout->windows[2].title, 'X', sizeof(fixture.layout->windows[2].title));
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) ==
        UMI_STATUS_INVALID_ARGUMENT);
    (void)g_strlcpy(fixture.layout->windows[2].title, "hidden",
        sizeof(fixture.layout->windows[2].title));
    fixture.layout->windows[1].width = 0.0;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) ==
        UMI_STATUS_INVALID_STATE);
    fixture.layout->windows[1].width = second.width;
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    REQUIRE(snapshot.canvas_count == 2U && snapshot.panel_count == 2U);
    REQUIRE(snapshot.source_layout_revision == source_revision && snapshot.geometry_pending);
    REQUIRE(umi_gtk4_workspace_layout_host_widget(fixture.host) == root);
    REQUIRE(find_tag(root, "workstation.canvas.panel.alpha") == panel);
    REQUIRE(fixture.created_content == 2U && fixture.released_content == 0U);
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "beta", &changed, source_revision) == UMI_STATUS_BUSY);
    drain_ready_callbacks();
    REQUIRE(fixture.requests == 1U);
    REQUIRE(strcmp(fixture.requested_window, "alpha") == 0);
    REQUIRE(fixture.expected_revision == source_revision);
    REQUIRE(fixture.requested_rect.x == changed.x && fixture.requested_rect.y == changed.y);
    REQUIRE(fixture.requested_rect.width == changed.width &&
        fixture.requested_rect.height == changed.height);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    REQUIRE(!snapshot.geometry_pending && snapshot.source_layout_revision == source_revision);

    /* A rejected owner request leaves the visible rectangle at the model's
     * value rather than keeping an uncommitted drag preview on screen. */
    fixture.response = UMI_STATUS_PERMISSION_DENIED;
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &changed, source_revision) == UMI_STATUS_OK);
    drain_ready_callbacks();
    REQUIRE(fixture.requests == 2U);
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha",
        &observed) == UMI_STATUS_OK);
    REQUIRE(observed.x == original.x && observed.width == original.width);

    /* An accepted callback may rebuild immediately. A geometry-only change
     * must retain provider widgets so a move cannot erase an unsaved draft. */
    fixture.response = UMI_STATUS_OK;
    fixture.action = CANVAS_OWNER_REBUILD;
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &changed, source_revision) == UMI_STATUS_OK);
    drain_ready_callbacks();
    REQUIRE(fixture.requests == 3U && fixture.callback_status == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_widget(fixture.host) == root);
    REQUIRE(fixture.released_content == 0U && fixture.created_content == 2U);
    REQUIRE(find_tag(root, "workstation.canvas.panel.alpha") == panel);
    source_revision = fixture.layout->revision;
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    REQUIRE(snapshot.source_layout_revision == source_revision && !snapshot.geometry_pending);
    REQUIRE(umi_gtk4_workspace_layout_host_canvas_geometry(fixture.host, "alpha",
        &observed) == UMI_STATUS_OK);
    REQUIRE(observed.x == changed.x && observed.width == changed.width);

    /* An explicit rebuild of unchanged geometry is a provider refresh. Product
     * controllers use it to display new data, so it must not be optimized away. */
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    REQUIRE(fixture.released_content == 2U && fixture.created_content == 4U);

    /* Frame actions are also deferred and repeated clicks coalesce. A full
     * policy rebuild then invalidates the queued action from the old frame. */
    action_button = find_tag(root, "alpha.action.move");
    REQUIRE(GTK_IS_BUTTON(action_button));
    g_signal_emit_by_name(action_button, "clicked");
    g_signal_emit_by_name(action_button, "clicked");
    REQUIRE(fixture.action_requests == 0U);
    drain_ready_callbacks();
    REQUIRE(fixture.action_requests == 1U && fixture.last_action == UMI_WS_PANEL_ACTION_MOVE);
    g_signal_emit_by_name(action_button, "clicked");

    /* Rebuild cancels queued work. Changing a layout to locked must not let a
     * gesture from the earlier revision slip through its old idle callback. */
    before = fixture.requests;
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &original, source_revision) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_locked(fixture.layout, true) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    drain_ready_callbacks();
    REQUIRE(fixture.requests == before);
    REQUIRE(fixture.action_requests == 1U);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    REQUIRE(!snapshot.canvas_editable && !snapshot.geometry_pending);
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &original, fixture.layout->revision) == UMI_STATUS_PERMISSION_DENIED);
    REQUIRE(umi_ui_workspace_layout_set_locked(fixture.layout, false) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workspace_layout_set_pinned(fixture.layout, "alpha", true) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &original, fixture.layout->revision) == UMI_STATUS_PERMISSION_DENIED);

    /* A nonresizable panel may move while keeping its exact model dimensions. */
    REQUIRE(umi_ui_workspace_layout_set_pinned(fixture.layout, "alpha", false) == UMI_STATUS_OK);
    fixture.layout->windows[0].resizable = false;
    fixture.layout->revision += 1U;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &original, fixture.layout->revision) == UMI_STATUS_PERMISSION_DENIED);
    observed = changed;
    observed.x = 0.25;
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &observed, fixture.layout->revision) == UMI_STATUS_OK);
    /* Removing the handler cancels even an otherwise valid pending move. */
    REQUIRE(umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(
        fixture.host, NULL, NULL) == UMI_STATUS_OK);
    drain_ready_callbacks();
    REQUIRE(fixture.requests == before);

    /* Docked tab close requests use the same cancellation rule. Switching a
     * panel back to canvas removes its old tab before the idle can run. */
    (void)g_strlcpy(fixture.layout->windows[0].placement_id, "centre",
        sizeof(fixture.layout->windows[0].placement_id));
    fixture.layout->revision += 1U;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    action_button = find_tag(root, "alpha.tab.close");
    REQUIRE(GTK_IS_BUTTON(action_button));
    g_signal_emit_by_name(action_button, "clicked");
    (void)g_strlcpy(fixture.layout->windows[0].placement_id, UMI_UI_WORKSPACE_CANVAS_PLACEMENT,
        sizeof(fixture.layout->windows[0].placement_id));
    fixture.layout->revision += 1U;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    drain_ready_callbacks();
    REQUIRE(fixture.action_requests == 1U);

    /* Empty means no product panel creation and no hidden notebook scaffolding. */
    REQUIRE(umi_ui_workspace_layout_init(empty_layout, "test.empty", "Empty canvas") == UMI_STATUS_OK);
    created = fixture.created_content;
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, empty_layout) == UMI_STATUS_OK);
    REQUIRE(fixture.created_content == created && fixture.released_content == created);
    snapshot = umi_gtk4_workspace_layout_host_snapshot(fixture.host);
    REQUIRE(snapshot.canvas_count == 0U && snapshot.panel_count == 0U && snapshot.stack_count == 0U);
    REQUIRE(umi_gtk4_workspace_layout_host_window_count(fixture.host) == 0U);
    REQUIRE(find_tag(root, "workstation.workspace-canvas") != NULL);
    REQUIRE(!contains_notebook(root));

    /* The owner may close the host inside its callback. Retained GTK widgets
     * must then have no live route into the freed controller. */
    REQUIRE(umi_gtk4_workspace_layout_host_rebuild(fixture.host, fixture.layout) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(
        fixture.host, accept_geometry, &fixture) == UMI_STATUS_OK);
    retained_drag = find_tag(root, "workstation.canvas.drag.alpha");
    REQUIRE(retained_drag != NULL);
    g_object_ref(retained_drag);
    fixture.action = CANVAS_OWNER_DESTROY;
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &observed, fixture.layout->revision) == UMI_STATUS_OK);
    drain_ready_callbacks();
    REQUIRE(fixture.host == NULL && fixture.requests == before + 1U);
    emit_retained_drag(retained_drag);
    drain_ready_callbacks();
    REQUIRE(fixture.requests == before + 1U);
    g_object_unref(retained_drag);
    retained_drag = NULL;
    g_object_unref(root);
    root = NULL;
    REQUIRE(fixture.released_content == fixture.created_content);

    /* Closing before the idle callback runs cancels the request completely,
     * unlike closing from inside a callback that has already been delivered. */
    before = fixture.requests;
    (void)g_strlcpy(fixture.layout->windows[1].placement_id, "centre",
        sizeof(fixture.layout->windows[1].placement_id));
    fixture.layout->revision += 1U;
    REQUIRE(umi_gtk4_workspace_layout_host_create_interactive(fixture.layout, create_test_panel,
        &fixture, record_panel_action, &fixture, &fixture.host) == UMI_STATUS_OK);
    root = umi_gtk4_workspace_layout_host_widget(fixture.host);
    REQUIRE(root != NULL);
    g_object_ref(root);
    REQUIRE(umi_gtk4_workspace_layout_host_set_canvas_geometry_handler(
        fixture.host, accept_geometry, &fixture) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_workspace_layout_host_request_canvas_geometry(fixture.host,
        "alpha", &observed, fixture.layout->revision) == UMI_STATUS_OK);
    action_button = find_tag(root, "alpha.action.move");
    REQUIRE(GTK_IS_BUTTON(action_button));
    retained_frame_action = g_object_ref(action_button);
    action_button = find_tag(root, "beta.tab.close");
    REQUIRE(GTK_IS_BUTTON(action_button));
    retained_tab_action = g_object_ref(action_button);
    g_signal_emit_by_name(retained_frame_action, "clicked");
    g_signal_emit_by_name(retained_tab_action, "clicked");
    umi_gtk4_workspace_layout_host_destroy(fixture.host);
    fixture.host = NULL;
    drain_ready_callbacks();
    REQUIRE(fixture.requests == before);
    REQUIRE(fixture.action_requests == 1U);
    /* Outside observers may retain a button after its host closes. Its queued
     * work and any later clicks must both be disconnected from the old owner. */
    g_signal_emit_by_name(retained_frame_action, "clicked");
    g_signal_emit_by_name(retained_tab_action, "clicked");
    drain_ready_callbacks();
    REQUIRE(fixture.action_requests == 1U);
    g_clear_object(&retained_frame_action);
    g_clear_object(&retained_tab_action);
    g_object_unref(root);
    root = NULL;
    REQUIRE(fixture.released_content == fixture.created_content);

    /* Exercise three Framework product catalogues, not their standalone
     * executable entry points or their business-operation controllers. */
    REQUIRE(verify_suite_canvas("org.umicom.bank", "accounts") == 0);
    REQUIRE(verify_suite_canvas("org.umicom.trader", "chart") == 0);
    REQUIRE(verify_suite_canvas("org.umicom.studio", "editor") == 0);
    REQUIRE(verify_canvas_edit_controls() == 0);

cleanup:
    umi_gtk4_workspace_layout_host_destroy(fixture.host);
    if (gesture != NULL) g_object_unref(gesture);
    if (retained_drag != NULL) g_object_unref(retained_drag);
    if (retained_frame_action != NULL) g_object_unref(retained_frame_action);
    if (retained_tab_action != NULL) g_object_unref(retained_tab_action);
    if (root != NULL) g_object_unref(root);
    free(empty_layout);
    free(fixture.layout);
    return failed;
}
