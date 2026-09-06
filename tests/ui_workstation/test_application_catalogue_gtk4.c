/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_application_catalogue_gtk4.c
 *
 * PURPOSE:
 *   Exercise the shared application picker through C contracts and real GTK
 *   controls. The injected host never starts a process or opens a window.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/shell_header.h"
#include "umicom/ui/appearance_catalogue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Fail in release builds too: these checks must not disappear with NDEBUG. */
#define REQUIRE(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; \
        goto cleanup; \
    } \
} while (0)

typedef struct TestHost {
    UmiGtk4WorkstationShellHeader *header;
    UmiApplicationLaunchDispatchReport *active_report;
    unsigned bank_calls;
    unsigned trader_calls;
    bool deny_bank;
    bool reject_reentry;
    bool destroy_on_request;
} TestHost;

/* Simulate independent host decisions without touching executable files.
 * Reentrant selection changes must be rejected while Open selected is running. */
static UmiStatus accept_application(
    const char *application_id,
    UmiGtk4WorkstationApplicationOpenMode mode,
    void *user_data)
{
    TestHost *host = (TestHost *)user_data;
    (void)mode;
    if (host->reject_reentry &&
        umi_gtk4_ws_shell_header_catalogue_set_selected(
            host->header, "org.umicom.trader", true) != UMI_STATUS_BUSY) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    /* A nested caller might pass the same report. Refusing reentry must not
     * erase results already written by the outer dispatch. */
    if (host->reject_reentry && host->active_report != NULL) {
        size_t reported = host->active_report->result_count;
        if (umi_gtk4_ws_shell_header_catalogue_dispatch(
                host->header, host->active_report) != UMI_STATUS_BUSY ||
            host->active_report->result_count != reported) {
            return UMI_STATUS_INTERNAL_ERROR;
        }
    }
    /* Closing a host from its own callback must defer disposal until the
     * dispatch report is complete. Later requests must not call this host. */
    if (host->destroy_on_request) {
        host->destroy_on_request = false;
        umi_gtk4_ws_shell_header_destroy(host->header);
        host->header = NULL;
    }
    if (strcmp(application_id, "org.umicom.bank") == 0) {
        host->bank_calls += 1U;
        return host->deny_bank ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
    }
    if (strcmp(application_id, "org.umicom.trader") == 0) {
        host->trader_calls += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_OK;
}

/* Tags are the same stable identifiers used by the native automation driver.
 * The test avoids captions, screen coordinates and fragile child indexes. */
static GtkWidget *find_control(GtkWidget *widget, const char *automation_id)
{
    GtkWidget *child;
    const char *tag;
    if (widget == NULL) return NULL;
    tag = (const char *)g_object_get_data(
        G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, automation_id) == 0) return widget;
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find_control(child, automation_id);
        if (found != NULL) return found;
    }
    return NULL;
}

/* The approved mark is artwork, not angle-bracket characters in a label.
 * Inspect the whole header so a hidden fallback cannot later reappear. */
static bool contains_text_logo(GtkWidget *widget)
{
    GtkWidget *child;
    if (GTK_IS_LABEL(widget) &&
        strstr(gtk_label_get_text(GTK_LABEL(widget)), "<>") != NULL) {
        return true;
    }
    for (child = gtk_widget_get_first_child(widget); child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        if (contains_text_logo(child)) return true;
    }
    return false;
}

/* Measure both header styles without presenting a window. The source SVG is
 * deliberately much larger than the display icon: a minimum size request
 * alone must not allow that natural artwork size to make the title bar tall. */
static int verify_header_identity(bool compact)
{
    UmiGtk4WorkstationShellHeaderConfig config;
    UmiGtk4WorkstationShellHeader *header = NULL;
    UmiGtk4WorkstationShellHeaderSnapshot snapshot;
    UmiUiAppearanceProfile profile;
    GtkWidget *root;
    GtkWidget *icon;
    GtkWidget *title;
    GtkWidget *subtitle;
    GtkWidget *badge;
    GtkWidget *identity;
    GdkPaintable *paintable;
    GFile *expected_file = NULL;
    GFile *paintable_file = NULL;
    char *description_difference = NULL;
    char *unique_name = NULL;
    char *missing_icon = NULL;
    const char *tooltip;
    const char *initial_subtitle = "Trade capture and approvals";
    const char *updated_subtitle = "Settlement and exception review";
    int requested_width;
    int requested_height;
    int natural;
    int icon_size = compact ? 18 : 24;
    int failed = 0;

    config = umi_gtk4_ws_shell_header_config_default(
        "org.umicom.tms", "Umicom TMS");
    REQUIRE(config.compact);
    config.compact = compact;
    config.subtitle = initial_subtitle;
    config.mode_badge = "Simulation";
    REQUIRE(umi_gtk4_ws_shell_header_create_managed(&config, &header) ==
        UMI_STATUS_OK);
    root = umi_gtk4_ws_shell_header_widget(header);
    REQUIRE(root != NULL);
    icon = find_control(root, "workstation.identity.icon");
    title = find_control(root, "workstation.identity.title");
    subtitle = find_control(root, "workstation.identity.subtitle");
    badge = find_control(root, "workstation.identity.mode");
    REQUIRE(GTK_IS_PICTURE(icon));
    REQUIRE(GTK_IS_LABEL(title) && GTK_IS_LABEL(subtitle) && GTK_IS_LABEL(badge));
    REQUIRE(!contains_text_logo(root));
    REQUIRE(gtk_label_get_single_line_mode(GTK_LABEL(title)));
    REQUIRE(!gtk_label_get_wrap(GTK_LABEL(title)));
    REQUIRE(gtk_label_get_ellipsize(GTK_LABEL(title)) == PANGO_ELLIPSIZE_END);
    REQUIRE(gtk_widget_get_valign(icon) == GTK_ALIGN_CENTER);
    REQUIRE(gtk_widget_get_valign(title) == GTK_ALIGN_CENTER);
    REQUIRE(gtk_widget_get_valign(badge) == GTK_ALIGN_CENTER);
    REQUIRE(strcmp(gtk_label_get_text(GTK_LABEL(title)), config.title) == 0);
    REQUIRE(strcmp(gtk_label_get_text(GTK_LABEL(subtitle)), initial_subtitle) == 0);
    REQUIRE(gtk_widget_get_visible(subtitle) == !compact);
    snapshot = umi_gtk4_ws_shell_header_snapshot(header);
    REQUIRE(snapshot.compact == (compact ? 1 : 0));
    REQUIRE(strcmp(snapshot.subtitle, initial_subtitle) == 0);
    gtk_widget_get_size_request(icon, &requested_width, &requested_height);
    REQUIRE(requested_width == icon_size && requested_height == icon_size);

    /* Hiding the second line must not remove its explanation from the
     * tooltip or the description read by assistive technology. */
    identity = gtk_widget_get_parent(icon);
    REQUIRE(identity != NULL);
    tooltip = gtk_widget_get_tooltip_text(identity);
    REQUIRE(tooltip != NULL && strstr(tooltip, initial_subtitle) != NULL);
    description_difference = gtk_test_accessible_check_property(
        GTK_ACCESSIBLE(root), GTK_ACCESSIBLE_PROPERTY_DESCRIPTION, initial_subtitle);
    REQUIRE(description_difference == NULL);

    /* Use the exact checked-in vector fixture, never a generated substitute.
     * The paintable owns its file reference; this check compares that reference
     * and logical dimensions, not rendered pixels or a visible screenshot. */
    REQUIRE(g_file_test(UMICOM_TEST_BRAND_ICON_PATH, G_FILE_TEST_IS_REGULAR));
    REQUIRE(umi_ui_appearance_catalogue_find("umicom-dark", &profile) == UMI_STATUS_OK);
    REQUIRE(strlen(UMICOM_TEST_BRAND_ICON_PATH) < sizeof(profile.icon_resource));
    memcpy(profile.icon_resource, UMICOM_TEST_BRAND_ICON_PATH,
        strlen(UMICOM_TEST_BRAND_ICON_PATH) + 1U);
    REQUIRE(umi_gtk4_ws_shell_header_apply_appearance(header, &profile) == UMI_STATUS_OK);
    snapshot = umi_gtk4_ws_shell_header_snapshot(header);
    REQUIRE(snapshot.icon_visible == 1 && gtk_widget_get_visible(icon));
    paintable = gtk_picture_get_paintable(GTK_PICTURE(icon));
    REQUIRE(GTK_IS_ICON_PAINTABLE(paintable));
    expected_file = g_file_new_for_path(UMICOM_TEST_BRAND_ICON_PATH);
    /* This getter returns an owned reference, unlike GtkPicture's borrowed
     * paintable getter. Release it in cleanup even when a later check fails. */
    paintable_file = gtk_icon_paintable_get_file(GTK_ICON_PAINTABLE(paintable));
    REQUIRE(paintable_file != NULL);
    REQUIRE(g_file_equal(expected_file, paintable_file));
    REQUIRE(gdk_paintable_get_intrinsic_width(paintable) > 0);
    REQUIRE(gdk_paintable_get_intrinsic_width(paintable) <= icon_size);
    REQUIRE(gdk_paintable_get_intrinsic_height(paintable) > 0);
    REQUIRE(gdk_paintable_get_intrinsic_height(paintable) <= icon_size);
    gtk_widget_measure(icon, GTK_ORIENTATION_HORIZONTAL, -1,
        NULL, &natural, NULL, NULL);
    REQUIRE(natural == icon_size);
    gtk_widget_measure(icon, GTK_ORIENTATION_VERTICAL, -1,
        NULL, &natural, NULL, NULL);
    REQUIRE(natural == icon_size);

    /* Changing application text must preserve the chosen compactness. A
     * noncompact header can also hide and restore an optional subtitle. */
    REQUIRE(umi_gtk4_ws_shell_header_set_text(header, "Umicom TMS", "", "") ==
        UMI_STATUS_OK);
    REQUIRE(!gtk_widget_get_visible(subtitle) && !gtk_widget_get_visible(badge));
    REQUIRE(umi_gtk4_ws_shell_header_set_text(header, "Umicom TMS",
        updated_subtitle, "Paper") == UMI_STATUS_OK);
    REQUIRE(gtk_widget_get_visible(subtitle) == !compact);
    REQUIRE(gtk_widget_get_visible(badge));
    REQUIRE(strcmp(gtk_label_get_text(GTK_LABEL(subtitle)), updated_subtitle) == 0);
    REQUIRE(strcmp(gtk_label_get_text(GTK_LABEL(badge)), "Paper") == 0);
    snapshot = umi_gtk4_ws_shell_header_snapshot(header);
    REQUIRE(strcmp(snapshot.subtitle, updated_subtitle) == 0);
    tooltip = gtk_widget_get_tooltip_text(identity);
    REQUIRE(tooltip != NULL && strstr(tooltip, updated_subtitle) != NULL);
    description_difference = gtk_test_accessible_check_property(
        GTK_ACCESSIBLE(root), GTK_ACCESSIBLE_PROPERTY_DESCRIPTION, updated_subtitle);
    REQUIRE(description_difference == NULL);

    /* A unique nonexistent path makes this independent of installed assets.
     * Nothing is created on disk; explicit missing artwork must stay hidden. */
    unique_name = g_uuid_string_random();
    missing_icon = g_build_filename(g_get_tmp_dir(), unique_name, "umicom-icon.svg", NULL);
    REQUIRE(!g_file_test(missing_icon, G_FILE_TEST_EXISTS));
    REQUIRE(strlen(missing_icon) < sizeof(profile.icon_resource));
    memcpy(profile.icon_resource, missing_icon, strlen(missing_icon) + 1U);
    REQUIRE(umi_gtk4_ws_shell_header_apply_appearance(header, &profile) == UMI_STATUS_OK);
    snapshot = umi_gtk4_ws_shell_header_snapshot(header);
    REQUIRE(snapshot.icon_visible == 0);
    REQUIRE(!gtk_widget_get_visible(icon));
    REQUIRE(gtk_picture_get_paintable(GTK_PICTURE(icon)) == NULL);
    REQUIRE(!contains_text_logo(root));

cleanup:
    g_free(description_difference);
    g_free(missing_icon);
    g_free(unique_name);
    if (paintable_file != NULL) g_object_unref(paintable_file);
    if (expected_file != NULL) g_object_unref(expected_file);
    umi_gtk4_ws_shell_header_destroy(header);
    return failed;
}

/* GTK needs a display connection to construct widgets. Missing displays are
 * explicitly skipped; there is no presented window, real launcher or network. */
int main(void)
{
    UmiGtk4WorkstationShellHeaderConfig config;
    UmiGtk4WorkstationShellHeader *header = NULL;
    UmiApplicationLaunchDispatchReport *report = NULL;
    UmiApplicationLaunchSelectionSnapshot snapshot;
    GtkWidget *root = NULL;
    GtkWidget *legacy = NULL;
    GtkWidget *bank_checkbox;
    GtkWidget *open_button;
    GtkWidget *clear_button;
    GtkWidget *search;
    TestHost host = {0};
    int failed = 0;

    /* The test accessibility backend keeps description checks deterministic.
     * This affects only this process, not the user's desktop configuration. */
    REQUIRE(g_setenv("GTK_A11Y", "test", TRUE));
    if (!gtk_init_check()) {
        (void)puts("SKIP: GTK display is unavailable.");
        return 77;
    }
    REQUIRE(verify_header_identity(true) == 0);
    REQUIRE(verify_header_identity(false) == 0);
    /* Older callers still receive a floating widget which a GTK parent can
     * adopt. Adding managed lifetime protection must not break that contract. */
    legacy = umi_gtk4_ws_shell_header_create("Umicom Workstation", "", "", true);
    REQUIRE(legacy != NULL);
    REQUIRE(g_object_is_floating(legacy));
    g_object_ref_sink(legacy);
    g_object_unref(legacy);
    legacy = NULL;
    /* Result storage grows with the portfolio, so keep it off the C stack. */
    report = calloc(1U, sizeof(*report));
    REQUIRE(report != NULL);
    config = umi_gtk4_ws_shell_header_config_default(
        "org.umicom.studio", "Umicom Studio IDE");
    REQUIRE(umi_gtk4_ws_shell_header_create_managed(&config, &header) ==
        UMI_STATUS_OK);
    root = umi_gtk4_ws_shell_header_widget(header);
    REQUIRE(root != NULL);
    /* Keep the widget tree after controller disposal to test signal cleanup. */
    g_object_ref(root);
    host.header = header;
    host.deny_bank = true;
    REQUIRE(umi_gtk4_ws_shell_header_set_application_open_handler(
        header, accept_application, &host) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_refresh(header) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_snapshot(header, &snapshot) ==
        UMI_STATUS_OK);
    REQUIRE(snapshot.selected_count == 0U);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_dispatch(header, report) ==
        UMI_STATUS_INVALID_STATE);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_set_selected(
        header, "org.umicom.missing", true) == UMI_STATUS_NOT_FOUND);

    bank_checkbox = find_control(root,
        "workstation.application.select.org.umicom.bank");
    open_button = find_control(root, "workstation.application.open-selected");
    clear_button = find_control(root, "workstation.application.clear");
    search = find_control(root, "workstation.application.search");
    REQUIRE(GTK_IS_CHECK_BUTTON(bank_checkbox));
    REQUIRE(GTK_IS_BUTTON(open_button));
    REQUIRE(GTK_IS_BUTTON(clear_button));
    REQUIRE(GTK_IS_EDITABLE(search));

    /* A real checkbox signal and the public method feed the same C model. */
    gtk_check_button_set_active(GTK_CHECK_BUTTON(bank_checkbox), TRUE);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_set_selected(
        header, "org.umicom.trader", true) == UMI_STATUS_OK);
    gtk_editable_set_text(GTK_EDITABLE(search), "no matching application");
    g_signal_emit_by_name(search, "search-changed");
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_snapshot(header, &snapshot) ==
        UMI_STATUS_OK);
    REQUIRE(snapshot.selected_count == 2U);
    gtk_editable_set_text(GTK_EDITABLE(search), "");
    g_signal_emit_by_name(search, "search-changed");

    host.reject_reentry = true;
    host.active_report = report;
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_dispatch(header, report) ==
        UMI_STATUS_UNAVAILABLE);
    REQUIRE(report->result_count == 2U);
    REQUIRE(report->accepted_count == 1U && report->failed_count == 1U);
    REQUIRE(host.bank_calls == 1U && host.trader_calls == 1U);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_snapshot(header, &snapshot) ==
        UMI_STATUS_OK);
    REQUIRE(snapshot.selected_count == 1U);

    /* Retrying through the actual footer button must not reopen Trader. */
    host.deny_bank = false;
    host.active_report = NULL;
    g_signal_emit_by_name(open_button, "clicked");
    REQUIRE(host.bank_calls == 2U && host.trader_calls == 1U);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_snapshot(header, &snapshot) ==
        UMI_STATUS_OK);
    REQUIRE(snapshot.selected_count == 0U);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_set_selected(
        header, "org.umicom.desktop", true) == UMI_STATUS_OK);
    g_signal_emit_by_name(clear_button, "clicked");
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_snapshot(header, &snapshot) ==
        UMI_STATUS_OK);
    REQUIRE(snapshot.selected_count == 0U);

    /* A retained tree must not call a released controller after shutdown. */
    umi_gtk4_ws_shell_header_destroy(header);
    header = NULL;
    host.header = NULL;
    g_signal_emit_by_name(open_button, "clicked");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(bank_checkbox), TRUE);
    REQUIRE(host.bank_calls == 2U && host.trader_calls == 1U);

    /* Repeat with teardown inside a callback. Retain the tree independently,
     * but never dereference the disposed controller after dispatch returns. */
    g_object_unref(root);
    root = NULL;
    host = (TestHost){0};
    REQUIRE(umi_gtk4_ws_shell_header_create_managed(&config, &header) ==
        UMI_STATUS_OK);
    root = umi_gtk4_ws_shell_header_widget(header);
    REQUIRE(root != NULL);
    g_object_ref(root);
    host.header = header;
    REQUIRE(umi_gtk4_ws_shell_header_set_application_open_handler(
        header, accept_application, &host) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_set_selected(
        header, "org.umicom.bank", true) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_ws_shell_header_catalogue_set_selected(
        header, "org.umicom.trader", true) == UMI_STATUS_OK);
    host.destroy_on_request = true;
    {
        UmiStatus dispatch_status =
            umi_gtk4_ws_shell_header_catalogue_dispatch(header, report);
        /* Null before any failing assertion can jump into cleanup. */
        header = NULL;
        REQUIRE(dispatch_status == UMI_STATUS_CANCELLED);
    }
    REQUIRE(report->result_count == 2U);
    REQUIRE(report->accepted_count == 1U && report->failed_count == 1U);
    REQUIRE(host.bank_calls + host.trader_calls == 1U);

cleanup:
    umi_gtk4_ws_shell_header_destroy(header);
    if (root != NULL) g_object_unref(root);
    if (legacy != NULL) {
        if (g_object_is_floating(legacy)) g_object_ref_sink(legacy);
        g_object_unref(legacy);
    }
    free(report);
    return failed;
}
