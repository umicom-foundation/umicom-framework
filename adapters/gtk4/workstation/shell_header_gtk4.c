/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/shell_header_gtk4.c
 *
 * PURPOSE:
 *   Present one reusable, SVG-aware application identity in native GTK4
 *   workstation headers. Studio, Trader and future applications can therefore
 *   share the same branding, accessibility and appearance-change behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/shell_header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef G_OS_WIN32
#include <glib/gwin32.h>
#endif

struct UmiGtk4WorkstationShellHeader {
    GtkWidget *root;
    GtkWidget *icon;
    GtkWidget *title;
    GtkWidget *subtitle;
    GtkWidget *badge;
    char *resource_root;
    UmiGtk4WorkstationShellHeaderSnapshot state;
};

/* Copy public text into a bounded snapshot field and reject silent truncation. */
static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(destination, capacity, "%s", source);
    return written < 0 || (size_t)written >= capacity
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/* Return the running executable's directory so installed resources are found
 * even when an application is launched from a shortcut or another folder. */
static char *executable_directory(void)
{
#ifdef G_OS_WIN32
    return g_win32_get_package_installation_directory_of_module(NULL);
#else
    GError *error = NULL;
    char *executable = g_file_read_link("/proc/self/exe", &error);
    char *directory = executable != NULL
        ? g_path_get_dirname(executable)
        : NULL;

    g_free(executable);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (error != NULL) g_error_free(error);
    return directory;
#endif
}

/* Resolve a logical appearance resource in a predictable order. A direct path
 * helps development trees, an explicit root helps embedders, and the executable
 * directory is the normal installed-application location. */
static char *resolve_resource(
    const UmiGtk4WorkstationShellHeader *header,
    const char *resource)
{
    char *directory;
    char *candidate;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header == NULL || resource == NULL || resource[0] == '\0') {
        return NULL;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (g_file_test(resource, G_FILE_TEST_IS_REGULAR)) {
        return g_strdup(resource);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header->resource_root != NULL && header->resource_root[0] != '\0') {
        candidate = g_build_filename(header->resource_root, resource, NULL);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (candidate != NULL &&
            g_file_test(candidate, G_FILE_TEST_IS_REGULAR)) {
            return candidate;
        }
        g_free(candidate);
    }

    directory = executable_directory();
    candidate = directory != NULL
        ? g_build_filename(directory, resource, NULL)
        : NULL;
    g_free(directory);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (candidate != NULL &&
        g_file_test(candidate, G_FILE_TEST_IS_REGULAR)) {
        return candidate;
    }
    g_free(candidate);
    return NULL;
}

/* Update a label and hide it when its optional text is empty. Hiding empty
 * labels keeps compact application bars aligned without losing their widgets. */
static void update_optional_label(GtkWidget *label, const char *text)
{
    const char *safe_text = text != NULL ? text : "";

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (label == NULL) return;
    gtk_label_set_text(GTK_LABEL(label), safe_text);
    gtk_widget_set_visible(label, safe_text[0] != '\0');
}

/* Return common creation values. Callers only need to provide a stable
 * application identifier and the name that people should see. */
UmiGtk4WorkstationShellHeaderConfig
umi_gtk4_ws_shell_header_config_default(
    const char *application_id,
    const char *title)
{
    UmiGtk4WorkstationShellHeaderConfig config;

    config.application_id = application_id;
    config.title = title;
    config.subtitle = "";
    config.mode_badge = "";
    config.resource_root = NULL;
    config.compact = true;
    return config;
}

/* Create the reusable widget once. Later appearance and text changes update
 * these same child widgets instead of rebuilding the surrounding application. */
UmiStatus umi_gtk4_ws_shell_header_create_managed(
    const UmiGtk4WorkstationShellHeaderConfig *config,
    UmiGtk4WorkstationShellHeader **out_header)
{
    UmiGtk4WorkstationShellHeader *header;
    GtkWidget *titles;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || out_header == NULL ||
        config->application_id == NULL || config->application_id[0] == '\0' ||
        config->title == NULL || config->title[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_header = NULL;
    header = calloc(1U, sizeof(*header));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = copy_text(
        header->state.application_id,
        sizeof(header->state.application_id),
        config->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    header->resource_root = config->resource_root != NULL
        ? g_strdup(config->resource_root)
        : NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config->resource_root != NULL && header->resource_root == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto fail;
    }

    header->root = gtk_box_new(
        GTK_ORIENTATION_HORIZONTAL,
        config->compact ? 6 : 10);
    header->icon = gtk_picture_new();
    titles = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    header->title = gtk_label_new("");
    header->subtitle = gtk_label_new("");
    header->badge = gtk_label_new("");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header->root == NULL || header->icon == NULL || titles == NULL ||
        header->title == NULL || header->subtitle == NULL ||
        header->badge == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto fail;
    }

    /* Generic class names let every product inherit one shared visual rule.
     * Applications may add a local class to the root for spacing only. */
    gtk_widget_add_css_class(header->root, "umicom-workstation-header");
    gtk_widget_add_css_class(header->root, "umicom-workstation-identity");
    gtk_widget_add_css_class(header->icon, "umicom-workstation-identity-icon");
    gtk_widget_add_css_class(header->title, "umicom-workstation-identity-title");
    gtk_widget_add_css_class(header->subtitle, "dim-label");
    gtk_widget_add_css_class(header->badge, "umicom-mode-badge");
    /* Apply this branch only when its contract condition is satisfied. */
    if (config->compact) {
        gtk_widget_add_css_class(header->root, "compact");
    }

    gtk_widget_set_size_request(
        header->icon,
        config->compact ? 18 : 24,
        config->compact ? 18 : 24);
    gtk_picture_set_can_shrink(GTK_PICTURE(header->icon), TRUE);
    gtk_widget_set_visible(header->icon, FALSE);
    gtk_label_set_xalign(GTK_LABEL(header->title), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(header->subtitle), 0.0F);
    gtk_label_set_ellipsize(
        GTK_LABEL(header->subtitle), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(titles, !config->compact);
    gtk_box_append(GTK_BOX(titles), header->title);
    gtk_box_append(GTK_BOX(titles), header->subtitle);
    gtk_box_append(GTK_BOX(header->root), header->icon);
    gtk_box_append(GTK_BOX(header->root), titles);
    gtk_box_append(GTK_BOX(header->root), header->badge);

    header->state.compact = config->compact ? 1 : 0;
    header->state.revision = 1U;
    status = umi_gtk4_ws_shell_header_set_text(
        header,
        config->title,
        config->subtitle,
        config->mode_badge);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto fail;
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(header->root),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        header->state.title,
        -1);
    *out_header = header;
    return UMI_STATUS_OK;

fail:
    /* Constructors normally succeed together, but an allocation failure may
     * leave an individual widget unparented. Release those widgets first, then
     * release the root and any children that were already appended to it. */
    if (header->icon != NULL &&
        gtk_widget_get_parent(header->icon) == NULL) {
        g_object_unref(header->icon);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header->title != NULL &&
        gtk_widget_get_parent(header->title) == NULL) {
        g_object_unref(header->title);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header->subtitle != NULL &&
        gtk_widget_get_parent(header->subtitle) == NULL) {
        g_object_unref(header->subtitle);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header->badge != NULL &&
        gtk_widget_get_parent(header->badge) == NULL) {
        g_object_unref(header->badge);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (titles != NULL && gtk_widget_get_parent(titles) == NULL) {
        g_object_unref(titles);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header->root != NULL) g_object_unref(header->root);
    g_free(header->resource_root);
    free(header);
    return status;
}

/* Release controller-owned text. GTK parents keep normal widget ownership. */
void umi_gtk4_ws_shell_header_destroy(
    UmiGtk4WorkstationShellHeader *header)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header == NULL) return;
    g_free(header->resource_root);
    header->resource_root = NULL;
    header->root = NULL;
    header->icon = NULL;
    header->title = NULL;
    header->subtitle = NULL;
    header->badge = NULL;
    free(header);
}

/* Return the borrowed root for normal GTK composition. */
GtkWidget *umi_gtk4_ws_shell_header_widget(
    UmiGtk4WorkstationShellHeader *header)
{
    return header != NULL ? header->root : NULL;
}

/* Apply the appearance's contrast-aware SVG. Failure to find a packaged file
 * is treated as a graceful text-only identity, not an application error. */
UmiStatus umi_gtk4_ws_shell_header_apply_appearance(
    UmiGtk4WorkstationShellHeader *header,
    const UmiUiAppearanceProfile *profile)
{
    char reason[192U];
    char *resolved;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header == NULL || profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_ui_appearance_profile_validate(
        profile, reason, sizeof(reason));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(
        header->state.icon_resource,
        sizeof(header->state.icon_resource),
        profile->icon_resource);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    resolved = resolve_resource(header, profile->icon_resource);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolved != NULL) {
        gtk_picture_set_filename(GTK_PICTURE(header->icon), resolved);
        gtk_widget_set_tooltip_text(header->icon, header->state.title);
        gtk_widget_set_visible(header->icon, TRUE);
        header->state.icon_visible = 1;
        g_free(resolved);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        gtk_picture_set_paintable(GTK_PICTURE(header->icon), NULL);
        gtk_widget_set_visible(header->icon, FALSE);
        header->state.icon_visible = 0;
    }
    header->state.revision += 1U;
    return UMI_STATUS_OK;
}

/* Update readable identity text as one operation so the snapshot and native
 * labels cannot temporarily disagree with each other. */
UmiStatus umi_gtk4_ws_shell_header_set_text(
    UmiGtk4WorkstationShellHeader *header,
    const char *title,
    const char *subtitle,
    const char *mode_badge)
{
    const char *safe_subtitle = subtitle != NULL ? subtitle : "";
    const char *safe_badge = mode_badge != NULL ? mode_badge : "";
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header == NULL || title == NULL || title[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_text(
        header->state.title, sizeof(header->state.title), title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            header->state.subtitle,
            sizeof(header->state.subtitle),
            safe_subtitle);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            header->state.mode_badge,
            sizeof(header->state.mode_badge),
            safe_badge);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    gtk_label_set_text(GTK_LABEL(header->title), header->state.title);
    update_optional_label(header->subtitle, header->state.subtitle);
    update_optional_label(header->badge, header->state.mode_badge);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(header->root),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        header->state.title,
        -1);
    header->state.revision += 1U;
    return UMI_STATUS_OK;
}

/* Copy the small controller state by value for diagnostics and tests. */
UmiGtk4WorkstationShellHeaderSnapshot
umi_gtk4_ws_shell_header_snapshot(
    const UmiGtk4WorkstationShellHeader *header)
{
    UmiGtk4WorkstationShellHeaderSnapshot snapshot;

    (void)memset(&snapshot, 0, sizeof(snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header != NULL) snapshot = header->state;
    return snapshot;
}

/* Preserve the original source API by creating a managed header and then
 * releasing only its update controller. The returned GTK root remains owned
 * through ordinary GTK parenting, exactly as it did before this enhancement. */
GtkWidget *umi_gtk4_ws_shell_header_create(
    const char *title,
    const char *subtitle,
    const char *mode_badge,
    bool compact)
{
    UmiGtk4WorkstationShellHeaderConfig config =
        umi_gtk4_ws_shell_header_config_default(
            "org.umicom.workstation",
            title != NULL ? title : "Umicom Workstation");
    UmiGtk4WorkstationShellHeader *header = NULL;
    GtkWidget *widget;

    config.subtitle = subtitle;
    config.mode_badge = mode_badge;
    config.compact = compact;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_gtk4_ws_shell_header_create_managed(
            &config, &header) != UMI_STATUS_OK) {
        return NULL;
    }
    widget = umi_gtk4_ws_shell_header_widget(header);
    umi_gtk4_ws_shell_header_destroy(header);
    return widget;
}
