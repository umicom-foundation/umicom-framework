/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/shell_header_gtk4.c
 *
 * PURPOSE:
 *   Present reusable, SVG-aware application identity and startup surfaces in
 *   native GTK4 workstations. All Umicom applications therefore share the
 *   same branding, accessibility and appearance-change behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/shell_header.h"

#include "umicom/application/portfolio.h"
#include "umicom/ui/gtk4/automation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef G_OS_WIN32
#include <glib/gwin32.h>
#endif

struct UmiGtk4WorkstationShellHeader {
    GtkWidget *root;
    GtkWidget *application_tab;
    GtkWidget *icon;
    GtkWidget *fallback_icon;
    GtkWidget *title;
    GtkWidget *subtitle;
    GtkWidget *badge;
    GtkWidget *application_catalogue_button;
    GtkWidget *application_catalogue_popover;
    GtkWidget *application_catalogue_search;
    GtkWidget *application_catalogue_list;
    GtkWidget *application_catalogue_status;
    GtkWidget *new_window_button;
    GtkWidget *close_button;
    UmiGtk4WorkstationApplicationOpenHandler application_open_handler;
    void *application_open_user_data;
    char *resource_root;
    UmiGtk4WorkstationShellHeaderSnapshot state;
};

struct UmiGtk4WorkstationStartupSplash {
    GtkWidget *root;
    GtkWidget *icon;
    GtkWidget *fallback_icon;
    GtkWidget *title;
    GtkWidget *subtitle;
    GtkWidget *status;
    GtkWidget *badge;
    GtkWidget *progress;
    char *resource_root;
    UmiGtk4WorkstationStartupSplashSnapshot state;
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

/* Resolve a logical resource in a predictable order. A direct path helps
 * development trees, an explicit root helps embedders, and the executable
 * directory is the normal installed-application location. */
static char *resolve_resource_from_root(
    const char *resource_root,
    const char *resource)
{
    char *directory;
    char *candidate;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resource == NULL || resource[0] == '\0') {
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
    if (resource_root != NULL && resource_root[0] != '\0') {
        candidate = g_build_filename(resource_root, resource, NULL);
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

/* Resolve one appearance resource using the header's configured root. */
static char *resolve_header_resource(
    const UmiGtk4WorkstationShellHeader *header,
    const char *resource)
{
    return header != NULL
        ? resolve_resource_from_root(header->resource_root, resource)
        : NULL;
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

/* Present one bounded launch result inside the application catalogue. */
static void set_application_catalogue_status(
    UmiGtk4WorkstationShellHeader *header,
    const char *message,
    bool error)
{
    const char *safe_message = message != NULL ? message : "";

    if (header == NULL || header->application_catalogue_status == NULL) return;
    gtk_label_set_text(
        GTK_LABEL(header->application_catalogue_status), safe_message);
    gtk_widget_set_visible(
        header->application_catalogue_status, safe_message[0] != '\0');
    if (error) {
        gtk_widget_add_css_class(
            header->application_catalogue_status, "error");
        if (header->application_catalogue_button != NULL) {
            gtk_menu_button_popup(
                GTK_MENU_BUTTON(header->application_catalogue_button));
        }
    } else {
        gtk_widget_remove_css_class(
            header->application_catalogue_status, "error");
    }
}

/* Return whether a UTF-8 catalogue record contains the current query. */
static bool application_search_matches(
    const char *search_text,
    const char *query)
{
    char *folded_search;
    char *folded_query;
    bool matches;

    if (query == NULL || query[0] == '\0') return true;
    if (search_text == NULL || search_text[0] == '\0') return false;
    folded_search = g_utf8_casefold(search_text, -1);
    folded_query = g_utf8_casefold(query, -1);
    if (folded_search == NULL || folded_query == NULL) {
        g_free(folded_search);
        g_free(folded_query);
        return false;
    }
    matches = strstr(folded_search, folded_query) != NULL;
    g_free(folded_search);
    g_free(folded_query);
    return matches;
}

/* Filter the Framework-owned application catalogue without changing its data. */
static void on_application_catalogue_search_changed(
    GtkSearchEntry *entry,
    gpointer user_data)
{
    UmiGtk4WorkstationShellHeader *header =
        (UmiGtk4WorkstationShellHeader *)user_data;
    const char *query;
    GtkWidget *row;
    size_t visible_count = 0U;

    if (header == NULL || header->application_catalogue_list == NULL) return;
    query = gtk_editable_get_text(GTK_EDITABLE(entry));
    for (row = gtk_widget_get_first_child(header->application_catalogue_list);
         row != NULL;
         row = gtk_widget_get_next_sibling(row)) {
        GtkWidget *button = GTK_IS_LIST_BOX_ROW(row)
            ? gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row))
            : row;
        const char *search_text = button != NULL
            ? (const char *)g_object_get_data(
                  G_OBJECT(button), "umicom-application-search")
            : NULL;
        bool visible = application_search_matches(search_text, query);

        gtk_widget_set_visible(row, visible);
        if (visible) visible_count += 1U;
    }
    if (visible_count == 0U) {
        set_application_catalogue_status(
            header, "No Umicom application matches this search.", false);
    } else {
        set_application_catalogue_status(header, "", false);
    }
}

/* Resolve one executable name beside the current application or on PATH. */
static char *find_application_executable(const char *executable_name)
{
    char *directory;
    char *candidate;

    if (executable_name == NULL || executable_name[0] == '\0') return NULL;
    if (g_path_is_absolute(executable_name) &&
        g_file_test(executable_name, G_FILE_TEST_IS_REGULAR)) {
        return g_strdup(executable_name);
    }

    directory = executable_directory();
    candidate = directory != NULL
        ? g_build_filename(directory, executable_name, NULL)
        : NULL;
    if (candidate != NULL &&
        g_file_test(candidate, G_FILE_TEST_IS_REGULAR)) {
        g_free(directory);
        return candidate;
    }
    g_free(candidate);
#ifdef G_OS_WIN32
    if (!g_str_has_suffix(executable_name, ".exe")) {
        char *windows_name = g_strconcat(executable_name, ".exe", NULL);

        candidate = directory != NULL && windows_name != NULL
            ? g_build_filename(directory, windows_name, NULL)
            : NULL;
        g_free(windows_name);
        if (candidate != NULL &&
            g_file_test(candidate, G_FILE_TEST_IS_REGULAR)) {
            g_free(directory);
            return candidate;
        }
        g_free(candidate);
    }
#endif
    g_free(directory);
    return g_find_program_in_path(executable_name);
}

/* Resolve the canonical name and supported packaged executable conventions. */
static char *resolve_application_executable(
    const UmiApplicationDefinition *application)
{
    const char *base_name;
    char *candidate_name;
    char *resolved;

    if (application == NULL) return NULL;
    if (application->executable_name != NULL &&
        !g_str_has_suffix(application->executable_name, "-console")) {
        resolved = find_application_executable(application->executable_name);
        if (resolved != NULL) return resolved;
    }

    base_name = application->repository_slug;
    if (base_name == NULL || base_name[0] == '\0') return NULL;
    resolved = find_application_executable(base_name);
    if (resolved != NULL) return resolved;

    candidate_name = g_strdup_printf("%s-gtk", base_name);
    resolved = find_application_executable(candidate_name);
    g_free(candidate_name);
    if (resolved != NULL) return resolved;

    candidate_name = g_strdup_printf("%s-ide", base_name);
    resolved = find_application_executable(candidate_name);
    g_free(candidate_name);
    if (resolved != NULL) return resolved;

    resolved = find_application_executable(application->executable_name);
    if (resolved != NULL) return resolved;

    candidate_name = g_strdup_printf("%s-console", base_name);
    resolved = find_application_executable(candidate_name);
    g_free(candidate_name);
    return resolved;
}

/* Start one independently runnable application from the canonical portfolio. */
static UmiStatus launch_portfolio_application(
    UmiGtk4WorkstationShellHeader *header,
    const UmiApplicationDefinition *application,
    UmiGtk4WorkstationApplicationOpenMode mode)
{
    GError *error = NULL;
    char *arguments[2];
    char *executable;
    char *message;
    UmiStatus status;
    gboolean started;

    if (header == NULL || application == NULL ||
        application->application_id == NULL ||
        application->display_name == NULL ||
        application->executable_name == NULL ||
        application->executable_name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (header->application_open_handler != NULL) {
        status = header->application_open_handler(
            application->application_id,
            mode,
            header->application_open_user_data);
        if (status != UMI_STATUS_OK) {
            message = g_strdup_printf(
                "Unable to open %s: the active host declined the request.",
                application->display_name);
            set_application_catalogue_status(
                header,
                message != NULL
                    ? message
                    : "The application could not be opened.",
                true);
            g_free(message);
            return status;
        }
        if (header->application_catalogue_button != NULL) {
            gtk_menu_button_popdown(
                GTK_MENU_BUTTON(header->application_catalogue_button));
        }
        return UMI_STATUS_OK;
    }

    executable = resolve_application_executable(application);
    if (executable == NULL) {
        message = g_strdup_printf(
            "Unable to open %s: %s was not found beside this application or on PATH.",
            application->display_name,
            application->executable_name);
        set_application_catalogue_status(
            header,
            message != NULL ? message : "The application executable was not found.",
            true);
        g_free(message);
        return UMI_STATUS_NOT_FOUND;
    }

    arguments[0] = executable;
    arguments[1] = NULL;
    started = g_spawn_async(
        NULL,
        arguments,
        NULL,
        G_SPAWN_DEFAULT,
        NULL,
        NULL,
        NULL,
        &error);
    if (!started) {
        message = g_strdup_printf(
            "Unable to open %s: %s",
            application->display_name,
            error != NULL ? error->message : "the executable is unavailable");
        set_application_catalogue_status(
            header,
            message != NULL ? message : "The application could not be opened.",
            true);
        g_free(message);
        g_free(executable);
        if (error != NULL) g_error_free(error);
        return UMI_STATUS_UNAVAILABLE;
    }

    message = g_strdup_printf("Opening %s…", application->display_name);
    set_application_catalogue_status(
        header, message != NULL ? message : "Opening application…", false);
    g_free(message);
    g_free(executable);
    if (header->application_catalogue_button != NULL) {
        gtk_menu_button_popdown(
            GTK_MENU_BUTTON(header->application_catalogue_button));
    }
    return UMI_STATUS_OK;
}

/* Open a catalogue selection through the host callback or default launcher. */
static void on_application_catalogue_item_clicked(
    GtkButton *button,
    gpointer user_data)
{
    UmiGtk4WorkstationShellHeader *header =
        (UmiGtk4WorkstationShellHeader *)user_data;
    const char *application_id;
    const UmiApplicationDefinition *application;

    if (header == NULL || button == NULL) return;
    application_id = (const char *)g_object_get_data(
        G_OBJECT(button), "umicom-application-id");
    application = application_id != NULL
        ? umi_application_portfolio_find(application_id)
        : NULL;
    if (application == NULL) {
        set_application_catalogue_status(
            header, "The selected application is no longer available.", true);
        return;
    }
    (void)launch_portfolio_application(
        header,
        application,
        UMI_GTK4_WORKSTATION_APPLICATION_OPEN_STANDARD);
}

/* Open another independent window for the current application. */
static void on_new_application_window_clicked(
    GtkButton *button,
    gpointer user_data)
{
    UmiGtk4WorkstationShellHeader *header =
        (UmiGtk4WorkstationShellHeader *)user_data;
    const UmiApplicationDefinition *application;

    (void)button;
    if (header == NULL) return;
    application = umi_application_portfolio_find(header->state.application_id);
    if (application == NULL) {
        set_application_catalogue_status(
            header, "This application has no runnable portfolio entry.", true);
        return;
    }
    (void)launch_portfolio_application(
        header,
        application,
        UMI_GTK4_WORKSTATION_APPLICATION_OPEN_NEW_WINDOW);
}

/* Close the top-level native application window which owns this header. */
static void on_close_application_clicked(
    GtkButton *button,
    gpointer user_data)
{
    UmiGtk4WorkstationShellHeader *header =
        (UmiGtk4WorkstationShellHeader *)user_data;
    GtkRoot *root;

    (void)button;
    if (header == NULL || header->root == NULL) return;
    root = gtk_widget_get_root(header->root);
    if (root != NULL && GTK_IS_WINDOW(root)) {
        gtk_window_close(GTK_WINDOW(root));
    }
}

/* Create a readable catalogue row from one canonical portfolio definition. */
static GtkWidget *create_application_catalogue_item(
    UmiGtk4WorkstationShellHeader *header,
    const UmiApplicationDefinition *application)
{
    GtkWidget *button;
    GtkWidget *content;
    GtkWidget *title;
    GtkWidget *purpose;
    char *search_text;
    char *automation_id;

    if (header == NULL || application == NULL ||
        application->application_id == NULL ||
        application->display_name == NULL) {
        return NULL;
    }
    button = gtk_button_new();
    content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    title = gtk_label_new(application->display_name);
    purpose = gtk_label_new(
        application->purpose != NULL ? application->purpose : "");
    if (button == NULL || content == NULL || title == NULL || purpose == NULL) {
        return NULL;
    }

    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(button, "umicom-application-catalogue-item");
    gtk_widget_set_hexpand(button, TRUE);
    gtk_widget_set_halign(content, GTK_ALIGN_FILL);
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(purpose), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(purpose), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(purpose), PANGO_WRAP_WORD_CHAR);
    gtk_widget_add_css_class(title, "heading");
    gtk_widget_add_css_class(purpose, "dim-label");
    gtk_box_append(GTK_BOX(content), title);
    if (application->purpose != NULL && application->purpose[0] != '\0') {
        gtk_box_append(GTK_BOX(content), purpose);
    }
    gtk_button_set_child(GTK_BUTTON(button), content);

    search_text = g_strconcat(
        application->display_name,
        " ",
        application->purpose != NULL ? application->purpose : "",
        " ",
        application->application_id,
        NULL);
    g_object_set_data_full(
        G_OBJECT(button),
        "umicom-application-id",
        g_strdup(application->application_id),
        g_free);
    g_object_set_data_full(
        G_OBJECT(button),
        "umicom-application-search",
        search_text,
        g_free);
    automation_id = g_strdup_printf(
        "workstation.application.%s", application->application_id);
    if (automation_id != NULL) {
        (void)umi_gtk4_automation_tag_widget(button, automation_id);
        g_free(automation_id);
    }
    gtk_widget_set_tooltip_text(
        button,
        application->purpose != NULL && application->purpose[0] != '\0'
            ? application->purpose
            : application->display_name);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(button),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        application->display_name,
        -1);
    g_signal_connect(
        button,
        "clicked",
        G_CALLBACK(on_application_catalogue_item_clicked),
        header);
    return button;
}

/* Populate the catalogue from the one canonical Framework application list. */
static UmiStatus populate_application_catalogue(
    UmiGtk4WorkstationShellHeader *header)
{
    size_t index;
    size_t added = 0U;

    if (header == NULL || header->application_catalogue_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < umi_application_portfolio_count(); ++index) {
        const UmiApplicationDefinition *application =
            umi_application_portfolio_at(index);
        GtkWidget *item;

        if (application == NULL) continue;
        if (umi_application_definition_validate(application) !=
            UMI_STATUS_OK) {
            set_application_catalogue_status(
                header,
                "The Framework application portfolio is invalid.",
                true);
            return UMI_STATUS_INVALID_STATE;
        }
        if ((application->flags & UMI_APPLICATION_STANDALONE) == 0U) {
            continue;
        }
        item = create_application_catalogue_item(header, application);
        if (item == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        gtk_list_box_append(
            GTK_LIST_BOX(header->application_catalogue_list), item);
        added += 1U;
    }
    if (added == 0U) {
        set_application_catalogue_status(
            header, "No runnable Umicom applications are registered.", true);
        return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}

/* Create the Framework-owned searchable application catalogue and controls. */
static UmiStatus create_application_controls(
    UmiGtk4WorkstationShellHeader *header)
{
    GtkWidget *catalogue_content;
    GtkWidget *catalogue_title;
    GtkWidget *scroll;
    UmiStatus status;

    if (header == NULL || header->root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    header->application_catalogue_button = gtk_menu_button_new();
    header->application_catalogue_popover = gtk_popover_new();
    header->application_catalogue_search = gtk_search_entry_new();
    header->application_catalogue_list = gtk_list_box_new();
    header->application_catalogue_status = gtk_label_new("");
    header->new_window_button = gtk_button_new_from_icon_name(
        "window-new-symbolic");
    header->close_button = gtk_button_new_from_icon_name(
        "window-close-symbolic");
    catalogue_content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    catalogue_title = gtk_label_new("Umicom Applications");
    scroll = gtk_scrolled_window_new();
    if (header->application_catalogue_button == NULL ||
        header->application_catalogue_popover == NULL ||
        header->application_catalogue_search == NULL ||
        header->application_catalogue_list == NULL ||
        header->application_catalogue_status == NULL ||
        header->new_window_button == NULL || header->close_button == NULL ||
        catalogue_content == NULL || catalogue_title == NULL || scroll == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    gtk_menu_button_set_icon_name(
        GTK_MENU_BUTTON(header->application_catalogue_button),
        "list-add-symbolic");
    gtk_widget_set_tooltip_text(
        header->application_catalogue_button,
        "Open an Umicom application");
    gtk_widget_set_tooltip_text(
        header->new_window_button,
        "Open another window for this application");
    gtk_widget_set_tooltip_text(
        header->close_button,
        "Close this application window");
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(header->application_catalogue_button),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Open Umicom application",
        -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(header->application_catalogue_search),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Search Umicom applications",
        -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(header->new_window_button),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Open another application window",
        -1);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(header->close_button),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        "Close application window",
        -1);
    gtk_widget_add_css_class(
        header->application_catalogue_button, "flat");
    gtk_widget_add_css_class(header->new_window_button, "flat");
    gtk_widget_add_css_class(header->close_button, "flat");
    gtk_widget_add_css_class(
        header->application_catalogue_button,
        "umicom-application-catalogue-button");
    gtk_widget_add_css_class(
        header->new_window_button, "umicom-application-window-button");
    gtk_widget_add_css_class(
        header->close_button, "umicom-application-close-button");
    gtk_widget_add_css_class(catalogue_title, "heading");
    gtk_widget_add_css_class(
        header->application_catalogue_status, "dim-label");
    gtk_label_set_xalign(GTK_LABEL(catalogue_title), 0.0F);
    gtk_label_set_xalign(
        GTK_LABEL(header->application_catalogue_status), 0.0F);
    gtk_label_set_wrap(
        GTK_LABEL(header->application_catalogue_status), TRUE);
    g_object_set(
        G_OBJECT(header->application_catalogue_search),
        "placeholder-text",
        "Search applications",
        NULL);
    gtk_list_box_set_selection_mode(
        GTK_LIST_BOX(header->application_catalogue_list),
        GTK_SELECTION_NONE);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_NEVER,
        GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_width(
        GTK_SCROLLED_WINDOW(scroll), 360);
    gtk_scrolled_window_set_min_content_height(
        GTK_SCROLLED_WINDOW(scroll), 320);
    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(scroll),
        header->application_catalogue_list);
    gtk_box_append(GTK_BOX(catalogue_content), catalogue_title);
    gtk_box_append(
        GTK_BOX(catalogue_content),
        header->application_catalogue_search);
    gtk_box_append(GTK_BOX(catalogue_content), scroll);
    gtk_box_append(
        GTK_BOX(catalogue_content),
        header->application_catalogue_status);
    gtk_popover_set_child(
        GTK_POPOVER(header->application_catalogue_popover),
        catalogue_content);
    gtk_menu_button_set_popover(
        GTK_MENU_BUTTON(header->application_catalogue_button),
        header->application_catalogue_popover);

    g_signal_connect(
        header->application_catalogue_search,
        "search-changed",
        G_CALLBACK(on_application_catalogue_search_changed),
        header);
    g_signal_connect(
        header->new_window_button,
        "clicked",
        G_CALLBACK(on_new_application_window_clicked),
        header);
    g_signal_connect(
        header->close_button,
        "clicked",
        G_CALLBACK(on_close_application_clicked),
        header);
    (void)umi_gtk4_automation_tag_widget(
        header->application_catalogue_button,
        "workstation.application.catalogue");
    (void)umi_gtk4_automation_tag_widget(
        header->application_catalogue_search,
        "workstation.application.search");
    (void)umi_gtk4_automation_tag_widget(
        header->new_window_button,
        "workstation.application.new-window");
    (void)umi_gtk4_automation_tag_widget(
        header->close_button,
        "workstation.application.close");

    status = populate_application_catalogue(header);
    if (status == UMI_STATUS_NOT_FOUND) {
        gtk_widget_set_sensitive(
            header->application_catalogue_button, FALSE);
        status = UMI_STATUS_OK;
    }
    gtk_widget_set_sensitive(
        header->new_window_button,
        umi_application_portfolio_find(header->state.application_id) != NULL);
    return status;
}

/* Disconnect callbacks which borrow a controller before releasing it. */
static void disconnect_header_callbacks(
    GtkWidget *widget,
    UmiGtk4WorkstationShellHeader *header)
{
    GtkWidget *child;

    if (widget == NULL || header == NULL) return;
    g_signal_handlers_disconnect_by_data(widget, header);
    for (child = gtk_widget_get_first_child(widget);
         child != NULL;
         child = gtk_widget_get_next_sibling(child)) {
        disconnect_header_callbacks(child, header);
    }
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
    GtkWidget *titles = NULL;
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
        config->compact ? 4 : 8);
    header->application_tab = gtk_box_new(
        GTK_ORIENTATION_HORIZONTAL,
        config->compact ? 6 : 10);
    header->icon = gtk_picture_new();
    header->fallback_icon = gtk_label_new("<>");
    titles = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    header->title = gtk_label_new("");
    header->subtitle = gtk_label_new("");
    header->badge = gtk_label_new("");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header->root == NULL || header->application_tab == NULL ||
        header->icon == NULL || header->fallback_icon == NULL ||
        titles == NULL ||
        header->title == NULL || header->subtitle == NULL ||
        header->badge == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto fail;
    }

    /* Generic class names let every product inherit one shared visual rule.
     * Applications may add a local class to the root for spacing only. */
    gtk_widget_add_css_class(header->root, "umicom-workstation-header");
    gtk_widget_add_css_class(header->root, "umicom-application-header");
    gtk_widget_add_css_class(
        header->application_tab, "umicom-application-active-tab");
    gtk_widget_add_css_class(
        header->application_tab, "umicom-workstation-identity");
    gtk_widget_add_css_class(header->icon, "umicom-workstation-identity-icon");
    gtk_widget_add_css_class(
        header->fallback_icon, "umicom-workstation-identity-fallback");
    gtk_widget_add_css_class(header->fallback_icon, "heading");
    gtk_widget_add_css_class(header->title, "umicom-workstation-identity-title");
    gtk_widget_add_css_class(header->subtitle, "dim-label");
    gtk_widget_add_css_class(header->badge, "umicom-mode-badge");
    /* Apply this branch only when its contract condition is satisfied. */
    if (config->compact) {
        gtk_widget_add_css_class(header->root, "compact");
        gtk_widget_add_css_class(header->application_tab, "compact");
    }

    gtk_widget_set_size_request(
        header->icon,
        config->compact ? 18 : 24,
        config->compact ? 18 : 24);
    gtk_picture_set_can_shrink(GTK_PICTURE(header->icon), TRUE);
    gtk_widget_set_visible(header->icon, FALSE);
    gtk_widget_set_visible(header->fallback_icon, TRUE);
    gtk_widget_set_tooltip_text(
        header->fallback_icon, "Umicom application identity");
    gtk_label_set_xalign(GTK_LABEL(header->title), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(header->subtitle), 0.0F);
    gtk_label_set_ellipsize(
        GTK_LABEL(header->subtitle), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(titles, TRUE);
    gtk_widget_set_hexpand(header->application_tab, TRUE);
    gtk_widget_set_halign(header->application_tab, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(titles), header->title);
    gtk_box_append(GTK_BOX(titles), header->subtitle);
    gtk_box_append(GTK_BOX(header->application_tab), header->icon);
    gtk_box_append(GTK_BOX(header->application_tab), header->fallback_icon);
    gtk_box_append(GTK_BOX(header->application_tab), titles);
    gtk_box_append(GTK_BOX(header->application_tab), header->badge);
    gtk_box_append(GTK_BOX(header->root), header->application_tab);

    status = create_application_controls(header);
    if (status != UMI_STATUS_OK) goto fail;
    gtk_box_append(
        GTK_BOX(header->root), header->application_catalogue_button);
    gtk_box_append(GTK_BOX(header->root), header->new_window_button);
    gtk_box_append(GTK_BOX(header->root), header->close_button);

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
    if (header->application_tab != NULL &&
        gtk_widget_get_parent(header->application_tab) == NULL) {
        g_object_unref(header->application_tab);
    }
    if (header->application_catalogue_button != NULL &&
        gtk_widget_get_parent(header->application_catalogue_button) == NULL) {
        g_object_unref(header->application_catalogue_button);
    }
    if (header->new_window_button != NULL &&
        gtk_widget_get_parent(header->new_window_button) == NULL) {
        g_object_unref(header->new_window_button);
    }
    if (header->close_button != NULL &&
        gtk_widget_get_parent(header->close_button) == NULL) {
        g_object_unref(header->close_button);
    }
    if (header->icon != NULL &&
        gtk_widget_get_parent(header->icon) == NULL) {
        g_object_unref(header->icon);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header->fallback_icon != NULL &&
        gtk_widget_get_parent(header->fallback_icon) == NULL) {
        g_object_unref(header->fallback_icon);
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
    disconnect_header_callbacks(
        header->application_catalogue_popover, header);
    disconnect_header_callbacks(header->root, header);
    g_free(header->resource_root);
    header->resource_root = NULL;
    header->application_open_handler = NULL;
    header->application_open_user_data = NULL;
    header->root = NULL;
    header->application_tab = NULL;
    header->icon = NULL;
    header->fallback_icon = NULL;
    header->title = NULL;
    header->subtitle = NULL;
    header->badge = NULL;
    header->application_catalogue_button = NULL;
    header->application_catalogue_popover = NULL;
    header->application_catalogue_search = NULL;
    header->application_catalogue_list = NULL;
    header->application_catalogue_status = NULL;
    header->new_window_button = NULL;
    header->close_button = NULL;
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

    resolved = resolve_header_resource(header, profile->icon_resource);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resolved != NULL) {
        gtk_picture_set_filename(GTK_PICTURE(header->icon), resolved);
        gtk_widget_set_tooltip_text(header->icon, header->state.title);
        gtk_widget_set_visible(header->icon, TRUE);
        gtk_widget_set_visible(header->fallback_icon, FALSE);
        header->state.icon_visible = 1;
        g_free(resolved);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        gtk_picture_set_paintable(GTK_PICTURE(header->icon), NULL);
        gtk_widget_set_visible(header->icon, FALSE);
        gtk_widget_set_visible(header->fallback_icon, TRUE);
        header->state.icon_visible = 1;
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
    gtk_widget_set_tooltip_text(
        header->application_tab, header->state.title);
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

/* Replace the default process launcher with a Framework host callback. */
UmiStatus umi_gtk4_ws_shell_header_set_application_open_handler(
    UmiGtk4WorkstationShellHeader *header,
    UmiGtk4WorkstationApplicationOpenHandler handler,
    void *user_data)
{
    if (header == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    header->application_open_handler = handler;
    header->application_open_user_data = user_data;
    header->state.revision += 1U;
    return UMI_STATUS_OK;
}

/* Show or conceal universal application controls without changing state. */
UmiStatus umi_gtk4_ws_shell_header_set_application_controls(
    UmiGtk4WorkstationShellHeader *header,
    bool show_catalogue,
    bool show_new_window,
    bool show_close)
{
    if (header == NULL || header->application_catalogue_button == NULL ||
        header->new_window_button == NULL || header->close_button == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    gtk_widget_set_visible(
        header->application_catalogue_button, show_catalogue);
    gtk_widget_set_visible(header->new_window_button, show_new_window);
    gtk_widget_set_visible(header->close_button, show_close);
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
    (void)umi_gtk4_ws_shell_header_set_application_controls(
        header, false, false, false);
    umi_gtk4_ws_shell_header_destroy(header);
    return widget;
}

/* Return consistent startup defaults while allowing every application to
 * provide product-specific status and mode text. */
UmiGtk4WorkstationStartupSplashConfig
umi_gtk4_ws_startup_splash_config_default(
    const char *application_id,
    const char *title)
{
    UmiGtk4WorkstationStartupSplashConfig config;

    config.application_id = application_id;
    config.title = title;
    config.subtitle = "Preparing your workspace";
    config.status = "Starting Umicom services…";
    config.mode_badge = "";
    config.resource_root = NULL;
    config.icon_resource = "branding/umicom-icon-on-dark.svg";
    return config;
}

/* Resolve and present the packaged mark, retaining a readable text mark when
 * an installation has no image resource. */
static void startup_splash_apply_icon(
    UmiGtk4WorkstationStartupSplash *splash,
    const char *icon_resource)
{
    char *resolved;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (splash == NULL) return;
    resolved = resolve_resource_from_root(
        splash->resource_root,
        icon_resource);
    /* Apply this branch only when its contract condition is satisfied. */
    if (resolved != NULL) {
        gtk_picture_set_filename(GTK_PICTURE(splash->icon), resolved);
        gtk_widget_set_visible(splash->icon, TRUE);
        gtk_widget_set_visible(splash->fallback_icon, FALSE);
        splash->state.icon_visible = 1;
        g_free(resolved);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        gtk_picture_set_paintable(GTK_PICTURE(splash->icon), NULL);
        gtk_widget_set_visible(splash->icon, FALSE);
        gtk_widget_set_visible(splash->fallback_icon, TRUE);
        splash->state.icon_visible = 1;
    }
}

/* Build one calm, centred startup surface. The window may be shown before
 * product services are constructed, ensuring visible feedback during startup. */
UmiStatus umi_gtk4_ws_startup_splash_create(
    const UmiGtk4WorkstationStartupSplashConfig *config,
    UmiGtk4WorkstationStartupSplash **out_splash)
{
    UmiGtk4WorkstationStartupSplash *splash;
    GtkWidget *hero;
    GtkWidget *identity;
    GtkWidget *status_group;
    GtkWidget *separator;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || out_splash == NULL ||
        config->application_id == NULL || config->application_id[0] == '\0' ||
        config->title == NULL || config->title[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_splash = NULL;
    splash = (UmiGtk4WorkstationStartupSplash *)calloc(
        1U, sizeof(*splash));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (splash == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = copy_text(
        splash->state.application_id,
        sizeof(splash->state.application_id),
        config->application_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            splash->state.title,
            sizeof(splash->state.title),
            config->title);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            splash->state.subtitle,
            sizeof(splash->state.subtitle),
            config->subtitle != NULL ? config->subtitle : "");
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            splash->state.status,
            sizeof(splash->state.status),
            config->status != NULL ? config->status : "");
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            splash->state.mode_badge,
            sizeof(splash->state.mode_badge),
            config->mode_badge != NULL ? config->mode_badge : "");
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            splash->state.icon_resource,
            sizeof(splash->state.icon_resource),
            config->icon_resource != NULL ? config->icon_resource : "");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(splash);
        return status;
    }

    splash->resource_root = config->resource_root != NULL
        ? g_strdup(config->resource_root)
        : NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config->resource_root != NULL && splash->resource_root == NULL) {
        free(splash);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    splash->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    hero = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);
    identity = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
    status_group = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    splash->icon = gtk_picture_new();
    splash->fallback_icon = gtk_label_new("<>");
    splash->title = gtk_label_new(splash->state.title);
    splash->subtitle = gtk_label_new(splash->state.subtitle);
    splash->status = gtk_label_new(splash->state.status);
    splash->badge = gtk_label_new(splash->state.mode_badge);
    splash->progress = gtk_progress_bar_new();
    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (splash->root == NULL || hero == NULL || identity == NULL ||
        status_group == NULL || splash->icon == NULL ||
        splash->fallback_icon == NULL || splash->title == NULL ||
        splash->subtitle == NULL || splash->status == NULL ||
        splash->badge == NULL || splash->progress == NULL ||
        separator == NULL) {
        g_free(splash->resource_root);
        /* Root owns any children already appended below; at this point none
         * have been appended, so release the created root and controller. */
        if (splash->root != NULL) g_object_unref(splash->root);
        free(splash);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    /* Keep an owning reference so the controller remains safe while the root
     * is swapped out of an application window. */
    g_object_ref_sink(splash->root);
    gtk_widget_add_css_class(splash->root, "umicom-startup-splash");
    gtk_widget_add_css_class(hero, "umicom-startup-hero");
    gtk_widget_add_css_class(identity, "umicom-startup-identity");
    gtk_widget_add_css_class(splash->icon, "umicom-startup-icon");
    gtk_widget_add_css_class(splash->fallback_icon, "title-1");
    gtk_widget_add_css_class(splash->title, "title-1");
    gtk_widget_add_css_class(splash->subtitle, "title-4");
    gtk_widget_add_css_class(splash->status, "dim-label");
    gtk_widget_add_css_class(splash->badge, "umicom-mode-badge");

    gtk_widget_set_halign(hero, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(hero, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(hero, TRUE);
    gtk_widget_set_vexpand(hero, TRUE);
    gtk_widget_set_halign(identity, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(splash->icon, 88, 88);
    gtk_picture_set_can_shrink(GTK_PICTURE(splash->icon), TRUE);
    gtk_label_set_wrap(GTK_LABEL(splash->subtitle), TRUE);
    gtk_label_set_wrap(GTK_LABEL(splash->status), TRUE);
    gtk_label_set_justify(GTK_LABEL(splash->subtitle), GTK_JUSTIFY_CENTER);
    gtk_label_set_justify(GTK_LABEL(splash->status), GTK_JUSTIFY_CENTER);
    gtk_label_set_xalign(GTK_LABEL(splash->subtitle), 0.5F);
    gtk_label_set_xalign(GTK_LABEL(splash->status), 0.5F);
    gtk_widget_set_halign(splash->badge, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(splash->progress, 360, -1);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(splash->progress), 0.0);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(splash->progress), FALSE);

    gtk_box_append(GTK_BOX(identity), splash->icon);
    gtk_box_append(GTK_BOX(identity), splash->fallback_icon);
    gtk_box_append(GTK_BOX(identity), splash->title);
    gtk_box_append(GTK_BOX(hero), identity);
    update_optional_label(splash->subtitle, splash->state.subtitle);
    gtk_box_append(GTK_BOX(hero), splash->subtitle);
    gtk_box_append(GTK_BOX(status_group), separator);
    update_optional_label(splash->status, splash->state.status);
    gtk_box_append(GTK_BOX(status_group), splash->status);
    gtk_box_append(GTK_BOX(status_group), splash->progress);
    update_optional_label(splash->badge, splash->state.mode_badge);
    gtk_box_append(GTK_BOX(status_group), splash->badge);
    gtk_box_append(GTK_BOX(hero), status_group);
    gtk_box_append(GTK_BOX(splash->root), hero);

    startup_splash_apply_icon(splash, splash->state.icon_resource);
    splash->state.progress = 0.0;
    splash->state.progress_visible = 1;
    splash->state.revision = 1U;
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(splash->root),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        splash->state.title,
        GTK_ACCESSIBLE_PROPERTY_DESCRIPTION,
        splash->state.status,
        -1);
    *out_splash = splash;
    return UMI_STATUS_OK;
}

/* Release the controller and its owning root reference. A parent keeps the
 * widget alive when it still belongs to a visible application window. */
void umi_gtk4_ws_startup_splash_destroy(
    UmiGtk4WorkstationStartupSplash *splash)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (splash == NULL) return;
    g_free(splash->resource_root);
    splash->resource_root = NULL;
    if (splash->root != NULL) {
        g_object_unref(splash->root);
        splash->root = NULL;
    }
    splash->icon = NULL;
    splash->fallback_icon = NULL;
    splash->title = NULL;
    splash->subtitle = NULL;
    splash->status = NULL;
    splash->badge = NULL;
    splash->progress = NULL;
    free(splash);
}

/* Return the borrowed root for normal application-window composition. */
GtkWidget *umi_gtk4_ws_startup_splash_widget(
    UmiGtk4WorkstationStartupSplash *splash)
{
    return splash != NULL ? splash->root : NULL;
}

/* Update status and mode as one operation so the accessibility description and
 * copied state always describe the same startup phase. */
UmiStatus umi_gtk4_ws_startup_splash_set_status(
    UmiGtk4WorkstationStartupSplash *splash,
    const char *status_text,
    const char *mode_badge)
{
    UmiStatus status;
    const char *safe_badge = mode_badge != NULL ? mode_badge : "";

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (splash == NULL || status_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_text(
        splash->state.status,
        sizeof(splash->state.status),
        status_text);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            splash->state.mode_badge,
            sizeof(splash->state.mode_badge),
            safe_badge);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    update_optional_label(splash->status, splash->state.status);
    update_optional_label(splash->badge, splash->state.mode_badge);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(splash->root),
        GTK_ACCESSIBLE_PROPERTY_DESCRIPTION,
        splash->state.status,
        -1);
    splash->state.revision += 1U;
    return UMI_STATUS_OK;
}

/* Clamp progress rather than accepting an invalid fraction from a partial
 * service initialisation path. */
UmiStatus umi_gtk4_ws_startup_splash_set_progress(
    UmiGtk4WorkstationStartupSplash *splash,
    double progress,
    int show_progress)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (splash == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (progress < 0.0) progress = 0.0;
    if (progress > 1.0) progress = 1.0;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(splash->progress), progress);
    gtk_widget_set_visible(splash->progress, show_progress != 0);
    splash->state.progress = progress;
    splash->state.progress_visible = show_progress != 0 ? 1 : 0;
    splash->state.revision += 1U;
    return UMI_STATUS_OK;
}

/* Copy startup presentation state by value for tests and diagnostics. */
UmiGtk4WorkstationStartupSplashSnapshot
umi_gtk4_ws_startup_splash_snapshot(
    const UmiGtk4WorkstationStartupSplash *splash)
{
    UmiGtk4WorkstationStartupSplashSnapshot snapshot;

    (void)memset(&snapshot, 0, sizeof(snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (splash != NULL) snapshot = splash->state;
    return snapshot;
}
