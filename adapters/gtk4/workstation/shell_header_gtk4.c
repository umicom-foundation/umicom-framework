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
#include <windows.h>
#ifdef GDK_WINDOWING_WIN32
#include <gdk/win32/gdkwin32.h>
#endif
#endif

struct UmiGtk4WorkstationShellHeader {
    GtkWidget *root;
    GtkWidget *application_tab;
    GtkWidget *icon;
    GtkWidget *title;
    GtkWidget *subtitle;
    GtkWidget *badge;
    GtkWidget *application_catalogue_button;
    GtkWidget *application_catalogue_popover;
    GtkWidget *application_catalogue_search;
    GtkWidget *application_catalogue_list;
    GtkWidget *application_catalogue_status;
    GtkWidget *application_selected_count;
    GtkWidget *application_open_selected;
    GtkWidget *application_clear_selection;
    GtkWidget *application_refresh;
    GtkWidget *new_window_button;
    GtkWidget *close_button;
    UmiGtk4WorkstationApplicationOpenHandler application_open_handler;
    void *application_open_user_data;
    UmiApplicationRuntimeCatalogue *application_runtime_catalogue;
    UmiApplicationLaunchSelection *application_selection;
    bool syncing_application_selection;
    bool dispatching_applications;
    bool destroy_pending;
    unsigned operation_depth;
    char *resource_root;
    UmiGtk4WorkstationShellHeaderSnapshot state;
};

/* Host callbacks may destroy their header synchronously. Active operations
 * retain the controller and its widget tree until their last callback returns. */
static void finish_header_operation(UmiGtk4WorkstationShellHeader *header);
static void sync_application_selection(UmiGtk4WorkstationShellHeader *header);

struct UmiGtk4WorkstationStartupSplash {
    GtkWidget *root;
    GtkWidget *icon;
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
    DWORD capacity = 256U;
    /* The GLib installation-prefix helper may strip a final bin directory.
     * Resolve the actual executable instead, using heap storage for long paths. */
    while (capacity <= 32768U) {
        WCHAR *buffer = g_new(WCHAR, capacity);
        DWORD length = GetModuleFileNameW(NULL, buffer, capacity);
        char *filename;
        char *directory;
        if (length == 0U) {
            g_free(buffer);
            return NULL;
        }
        if (length < capacity) {
            filename = g_utf16_to_utf8(
                (const gunichar2 *)buffer, (glong)length, NULL, NULL, NULL);
            g_free(buffer);
            if (filename == NULL) return NULL;
            directory = g_path_get_dirname(filename);
            g_free(filename);
            return directory;
        }
        g_free(buffer);
        capacity *= 2U;
    }
    return NULL;
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

/* Add the packaged mark to GTK's icon lookup. The display owns its theme. */
static UmiStatus apply_themed_window_identity(GtkWindow *window)
{
    GdkDisplay *display;
    GtkIconTheme *theme;
    char *resolved;
    char *absolute;
    char *directory;
    char **search_paths;
    size_t index;
    bool already_registered = false;

    if (window == NULL || !GTK_IS_WINDOW(window)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    display = gtk_widget_get_display(GTK_WIDGET(window));
    if (display == NULL) return UMI_STATUS_UNAVAILABLE;
    resolved = resolve_resource_from_root(NULL, "branding/umicom-icon.svg");
    if (resolved == NULL) return UMI_STATUS_NOT_FOUND;
    theme = gtk_icon_theme_get_for_display(display);
    if (theme == NULL) {
        g_free(resolved);
        return UMI_STATUS_UNAVAILABLE;
    }
    absolute = g_canonicalize_filename(resolved, NULL);
    directory = g_path_get_dirname(absolute);
    g_free(absolute);
    g_free(resolved);
    search_paths = gtk_icon_theme_get_search_path(theme);
    for (index = 0U; search_paths != NULL && search_paths[index] != NULL; ++index) {
        if (strcmp(search_paths[index], directory) == 0) {
            already_registered = true;
            break;
        }
    }
    if (!already_registered) gtk_icon_theme_add_search_path(theme, directory);
    g_strfreev(search_paths);
    g_free(directory);
    if (!gtk_icon_theme_has_icon(theme, "umicom-icon")) {
        return UMI_STATUS_NOT_FOUND;
    }
    gtk_window_set_icon_name(window, "umicom-icon");
    return UMI_STATUS_OK;
}

#if defined(G_OS_WIN32) && defined(GDK_WINDOWING_WIN32)
/* Each GTK window owns its two icon handles until finalization, after its
 * native surface is disposed. Never destroy previous WM_SETICON handles,
 * which may belong to GTK or to another platform resource owner. */
typedef struct NativeWindowIdentity {
    HICON small_icon;
    HICON large_icon;
} NativeWindowIdentity;

/* Release only the private icons loaded by this window's identity bridge. */
static void release_native_window_identity(gpointer data)
{
    NativeWindowIdentity *identity = data;

    if (identity->small_icon != NULL) (void)DestroyIcon(identity->small_icon);
    if (identity->large_icon != NULL) (void)DestroyIcon(identity->large_icon);
    g_free(identity);
}

/* The template embeds canonical ICO101 unchanged. Do not use LR_SHARED:
 * Windows caches that flag by resource name, not requested pixel dimensions. */
static UmiStatus load_native_window_icons(NativeWindowIdentity *identity)
{
    HINSTANCE module = GetModuleHandleW(NULL);

    if (module == NULL) return UMI_STATUS_UNAVAILABLE;
    identity->small_icon = (HICON)LoadImageW(module, MAKEINTRESOURCEW(101), IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    identity->large_icon = (HICON)LoadImageW(module, MAKEINTRESOURCEW(101), IMAGE_ICON,
        GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    return identity->small_icon != NULL && identity->large_icon != NULL
        ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/* A PE executable icon is not an HWND icon. Assign both native window sizes
 * after GTK creates its surface, without changing the caller's titlebar. */
static UmiStatus apply_native_window_identity(GtkWindow *window)
{
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(window));
    HWND handle;
    NativeWindowIdentity *identity = g_object_get_data(
        G_OBJECT(window), "umicom-native-window-identity");

    if (identity == NULL) return UMI_STATUS_INVALID_STATE;
    if (surface == NULL || !GDK_IS_WIN32_SURFACE(surface)) return UMI_STATUS_UNAVAILABLE;
    handle = gdk_win32_surface_get_handle(surface);
    if (handle == NULL) return UMI_STATUS_UNAVAILABLE;
    (void)SendMessageW(handle, WM_SETICON, ICON_SMALL, (LPARAM)identity->small_icon);
    (void)SendMessageW(handle, WM_SETICON, ICON_BIG, (LPARAM)identity->large_icon);
    return UMI_STATUS_OK;
}

/* Reapply after GTK's own realize/map handlers, including surface recreation.
 * The signal belongs to this window and borrows no application controller. */
static void on_native_window_identity_ready(GtkWidget *widget, gpointer user_data)
{
    (void)user_data;
    (void)apply_native_window_identity(GTK_WINDOW(widget));
}

/* Register only once, and never force realization or presentation just for an
 * icon. Missing resources leave the existing platform icon untouched. */
static UmiStatus prepare_native_window_identity(GtkWindow *window)
{
    NativeWindowIdentity *identity = g_object_get_data(
        G_OBJECT(window), "umicom-native-window-identity");

    if (identity == NULL) {
        UmiStatus status;
        identity = g_try_new0(NativeWindowIdentity, 1U);
        if (identity == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        status = load_native_window_icons(identity);
        if (status != UMI_STATUS_OK) {
            release_native_window_identity(identity);
            return status;
        }
        g_object_set_data_full(G_OBJECT(window), "umicom-native-window-identity",
            identity, release_native_window_identity);
        g_signal_connect_after(window, "realize",
            G_CALLBACK(on_native_window_identity_ready), NULL);
        g_signal_connect_after(window, "map",
            G_CALLBACK(on_native_window_identity_ready), NULL);
    }
    return gtk_widget_get_realized(GTK_WIDGET(window))
        ? apply_native_window_identity(window) : UMI_STATUS_OK;
}
#endif

/* Keep themed desktop identity and native Windows identity complementary.
 * Either canonical asset path may work when the other is unavailable. */
UmiStatus umi_gtk4_ws_apply_window_identity(GtkWindow *window)
{
    UmiStatus themed_status;

    if (window == NULL || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    themed_status = apply_themed_window_identity(window);
#if defined(G_OS_WIN32) && defined(GDK_WINDOWING_WIN32)
    if (GDK_IS_WIN32_DISPLAY(gtk_widget_get_display(GTK_WIDGET(window)))) {
        UmiStatus native_status = prepare_native_window_identity(window);
        if (native_status == UMI_STATUS_OK) return UMI_STATUS_OK;
        if (themed_status != UMI_STATUS_OK) return native_status;
    }
#endif
    return themed_status;
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

/* Give GTK a small icon-sized view of the original SVG. A picture's size
 * request is only a minimum: loading the raw 236-pixel artwork directly would
 * still give the header that large natural size. The paintable preserves the
 * vector file while reporting the intended logical size to the layout. */
static void update_header_icon(UmiGtk4WorkstationShellHeader *header)
{
    char *resolved;
    GtkIconPaintable *paintable = NULL;

    if (header == NULL || header->icon == NULL) return;
    resolved = resolve_header_resource(header, header->state.icon_resource);
    if (resolved != NULL) {
        GFile *file = g_file_new_for_path(resolved);
        paintable = gtk_icon_paintable_new_for_file(file,
            header->state.compact ? 18 : 24,
            gtk_widget_get_scale_factor(header->icon));
        g_object_unref(file);
        g_free(resolved);
    }
    /* Missing artwork stays empty. Neither angle-bracket text nor a stock
     * toolkit symbol is a replacement for the approved Umicom mark. */
    gtk_picture_set_paintable(GTK_PICTURE(header->icon),
        paintable != NULL ? GDK_PAINTABLE(paintable) : NULL);
    header->state.icon_visible = paintable != NULL ? 1 : 0;
    gtk_widget_set_visible(header->icon, paintable != NULL);
    gtk_widget_set_tooltip_text(header->icon, header->state.title);
    if (paintable != NULL) g_object_unref(paintable);
}

/* A move to another display keeps the same logical icon size but requests
 * artwork at that display's pixel density. No image asset is rewritten. */
static void on_header_icon_scale_changed(
    GtkWidget *icon, GParamSpec *property, gpointer user_data)
{
    UmiGtk4WorkstationShellHeader *header = user_data;
    (void)icon;
    (void)property;
    update_header_icon(header);
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
        if (!header->dispatching_applications &&
            header->application_catalogue_button != NULL &&
            gtk_widget_get_mapped(header->application_catalogue_button)) {
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

/* Conservatively recognise a runnable native file, not a shell-script fallback. */
static bool is_native_application_executable(const char *path)
{
    if (path == NULL || !g_file_test(path, G_FILE_TEST_IS_REGULAR)) return false;
#ifdef G_OS_WIN32
    return strlen(path) >= 4U &&
        g_ascii_strcasecmp(path + strlen(path) - 4U, ".exe") == 0;
#else
    return g_file_test(path, G_FILE_TEST_IS_EXECUTABLE);
#endif
}

/* Resolve the exact native executable beside this application or on PATH. */
static char *find_application_executable(const char *executable_name)
{
    char *directory;
    char *candidate;
    char *native_name;

    if (executable_name == NULL || executable_name[0] == '\0') return NULL;
#ifdef G_OS_WIN32
    native_name = g_str_has_suffix(executable_name, ".exe")
        ? g_strdup(executable_name) : g_strconcat(executable_name, ".exe", NULL);
#else
    native_name = g_strdup(executable_name);
#endif
    if (native_name == NULL) return NULL;
    if (g_path_is_absolute(native_name) && is_native_application_executable(native_name))
        return native_name;

    directory = executable_directory();
    candidate = directory != NULL
        ? g_build_filename(directory, native_name, NULL)
        : NULL;
    if (is_native_application_executable(candidate)) {
        g_free(directory);
        g_free(native_name);
        return candidate;
    }
    g_free(candidate);
    g_free(directory);
    candidate = g_find_program_in_path(native_name);
    g_free(native_name);
    if (is_native_application_executable(candidate)) return candidate;
    g_free(candidate);
    return NULL;
}

/* Resolve only the canonical native GUI companion. Console verification tools
 * must never become a fallback for a graphical application request. */
static char *resolve_application_executable(
    const UmiApplicationDefinition *application)
{
    return application != NULL
        ? find_application_executable(umi_application_portfolio_gui_executable(
              application->application_id))
        : NULL;
}

/* Catalogue rows store view references only. Selection and eligibility remain
 * in the Framework runtime catalogue and launch-selection model. */
static GtkWidget *find_application_catalogue_item(
    UmiGtk4WorkstationShellHeader *header,
    const char *application_id)
{
    GtkWidget *row;

    if (header == NULL || header->application_catalogue_list == NULL ||
        application_id == NULL) return NULL;
    for (row = gtk_widget_get_first_child(header->application_catalogue_list);
         row != NULL; row = gtk_widget_get_next_sibling(row)) {
        GtkWidget *item = GTK_IS_LIST_BOX_ROW(row)
            ? gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row)) : row;
        const char *id = item != NULL
            ? g_object_get_data(G_OBJECT(item), "umicom-application-id") : NULL;
        if (id != NULL && strcmp(id, application_id) == 0) return item;
    }
    return NULL;
}

/* Keep the last request result beside its application, including filtered rows. */
static void set_application_row_result(
    UmiGtk4WorkstationShellHeader *header,
    const char *application_id,
    UmiStatus status,
    const char *message)
{
    GtkWidget *item;
    GtkWidget *label;
    char fallback[256U];

    if (header == NULL || header->destroy_pending) return;
    item = find_application_catalogue_item(header, application_id);
    label = item != NULL
        ? g_object_get_data(G_OBJECT(item), "umicom-application-result") : NULL;
    if (label == NULL) return;
    if (message == NULL) {
        (void)snprintf(fallback, sizeof(fallback), "Request failed: %s",
                       umi_status_text(status));
        message = status == UMI_STATUS_OK
            ? "Launch request accepted; startup is not yet confirmed."
            : fallback;
    }
    gtk_label_set_text(GTK_LABEL(label), message);
    gtk_widget_set_visible(label, message[0] != '\0');
    if (status == UMI_STATUS_OK) gtk_widget_remove_css_class(label, "error");
    else gtk_widget_add_css_class(label, "error");
}

/* Discovery confirms only a canonical GUI executable, never runtime health.
 * A delegated host can accept requests without a local executable and remains
 * responsible for authorising or rejecting each dispatched request. */
static UmiStatus refresh_application_presence(
    UmiGtk4WorkstationShellHeader *header)
{
    size_t index;

    for (index = 0U; index < umi_application_portfolio_count(); ++index) {
        const UmiApplicationDefinition *application =
            umi_application_portfolio_at(index);
        char *executable;
        bool available;
        UmiStatus status;

        if (application == NULL) return UMI_STATUS_INVALID_STATE;
        executable = header->application_open_handler == NULL
            ? resolve_application_executable(application) : NULL;
        available = header->application_open_handler != NULL || executable != NULL;
        status = umi_application_runtime_catalogue_set_presence(
            header->application_runtime_catalogue, application->application_id,
            available, available, available);
        g_free(executable);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Project copied model state into existing widgets without rebuilding rows or
 * losing the search query. Signal guards prevent projection from selecting items. */
static void sync_application_selection(UmiGtk4WorkstationShellHeader *header)
{
    UmiApplicationLaunchSelectionSnapshot snapshot;
    GtkWidget *row;
    char count_text[96U];

    if (header == NULL || header->destroy_pending ||
        header->application_selection == NULL ||
        header->application_catalogue_list == NULL) return;
    if (umi_application_launch_selection_snapshot(
            header->application_selection, &snapshot) != UMI_STATUS_OK) return;
    header->syncing_application_selection = true;
    for (row = gtk_widget_get_first_child(header->application_catalogue_list);
         row != NULL; row = gtk_widget_get_next_sibling(row)) {
        GtkWidget *item = GTK_IS_LIST_BOX_ROW(row)
            ? gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row)) : row;
        const char *id = g_object_get_data(G_OBJECT(item), "umicom-application-id");
        GtkWidget *check = g_object_get_data(G_OBJECT(item), "umicom-application-check");
        GtkWidget *open = g_object_get_data(G_OBJECT(item), "umicom-application-open");
        GtkWidget *availability = g_object_get_data(
            G_OBJECT(item), "umicom-application-availability");
        UmiApplicationLaunchChoice choice;
        UmiApplicationRuntimeRecord record;
        const char *message;

        if (id == NULL || check == NULL || open == NULL || availability == NULL ||
            umi_application_launch_selection_find(
                header->application_selection, id, &choice) != UMI_STATUS_OK ||
            umi_application_runtime_catalogue_find(
                header->application_runtime_catalogue, id, &record) != UMI_STATUS_OK) {
            continue;
        }
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), choice.selected);
        gtk_widget_set_sensitive(check, !header->dispatching_applications &&
            (choice.eligible || choice.selected));
        gtk_widget_set_sensitive(open,
            !header->dispatching_applications && choice.eligible);
        if (!record.installed) {
            message = "Native GUI not found. Build or install it, then Refresh.";
        } else if (!choice.eligible) {
            message = choice.readiness_reason[0] != '\0'
                ? choice.readiness_reason : "The shared launch gate blocks this application.";
        } else if (header->application_open_handler != NULL) {
            message = "Opening is delegated to the active host; it may decline.";
        } else {
            message = "GUI executable found; startup and features are unverified.";
        }
        gtk_label_set_text(GTK_LABEL(availability), message);
    }
    header->syncing_application_selection = false;
    (void)snprintf(count_text, sizeof(count_text), "%zu selected (across all results)",
                   snapshot.selected_count);
    if (header->application_selected_count != NULL)
        gtk_label_set_text(GTK_LABEL(header->application_selected_count), count_text);
    if (header->application_open_selected != NULL)
        gtk_widget_set_sensitive(header->application_open_selected,
            !header->dispatching_applications && snapshot.selected_count > 0U);
    if (header->application_clear_selection != NULL)
        gtk_widget_set_sensitive(header->application_clear_selection,
            !header->dispatching_applications && snapshot.selected_count > 0U);
    if (header->application_refresh != NULL)
        gtk_widget_set_sensitive(header->application_refresh,
            !header->dispatching_applications);
}

/* Refresh shared presence and eligibility while retaining selections and rows. */
UmiStatus umi_gtk4_ws_shell_header_catalogue_refresh(
    UmiGtk4WorkstationShellHeader *header)
{
    UmiStatus status;

    if (header == NULL || header->destroy_pending ||
        header->application_selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (header->dispatching_applications) return UMI_STATUS_BUSY;
    header->operation_depth += 1U;
    status = refresh_application_presence(header);
    if (status == UMI_STATUS_OK)
        status = umi_application_launch_selection_refresh(header->application_selection);
    if (status == UMI_STATUS_OK) sync_application_selection(header);
    finish_header_operation(header);
    return status;
}

/* Apply one explicit selection change to the shared model and existing controls. */
UmiStatus umi_gtk4_ws_shell_header_catalogue_set_selected(
    UmiGtk4WorkstationShellHeader *header,
    const char *application_id,
    bool selected)
{
    UmiStatus status;

    if (header == NULL || header->destroy_pending ||
        header->application_selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (header->dispatching_applications) return UMI_STATUS_BUSY;
    header->operation_depth += 1U;
    status = umi_application_launch_selection_set_selected(
        header->application_selection, application_id, selected);
    if (status != UMI_STATUS_OK)
        set_application_row_result(header, application_id, status, NULL);
    sync_application_selection(header);
    finish_header_operation(header);
    return status;
}

/* Copy selection counts independently of the current search visibility. */
UmiStatus umi_gtk4_ws_shell_header_catalogue_snapshot(
    const UmiGtk4WorkstationShellHeader *header,
    UmiApplicationLaunchSelectionSnapshot *out_snapshot)
{
    if (header == NULL || header->destroy_pending || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_launch_selection_snapshot(
        header->application_selection, out_snapshot);
}

/* Start one independently runnable application from the canonical portfolio. */
static UmiStatus launch_portfolio_application(
    UmiGtk4WorkstationShellHeader *header,
    const UmiApplicationDefinition *application,
    UmiGtk4WorkstationApplicationOpenMode mode)
{
    GError *error = NULL;
    char *arguments[2];
    char *executable = NULL;
    char *message = NULL;
    UmiStatus status = UMI_STATUS_OK;
    gboolean started;

    if (header == NULL || header->destroy_pending || application == NULL ||
        application->application_id == NULL ||
        application->display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    header->operation_depth += 1U;
    if (mode == UMI_GTK4_WORKSTATION_APPLICATION_OPEN_STANDARD) {
        UmiApplicationLaunchChoice choice;
        status = umi_application_launch_selection_find(
            header->application_selection, application->application_id, &choice);
        if (status != UMI_STATUS_OK) goto reported;
        if (!choice.eligible) {
            status = UMI_STATUS_UNAVAILABLE;
            message = g_strdup(choice.readiness_reason[0] != '\0'
                ? choice.readiness_reason : "The shared launch gate blocks this application.");
            goto reported;
        }
    }
    if (header->application_open_handler != NULL) {
        status = header->application_open_handler(
            application->application_id,
            mode,
            header->application_open_user_data);
        if (header->destroy_pending) goto finished;
        if (status != UMI_STATUS_OK) {
            message = g_strdup_printf(
                "The active host declined %s: %s.",
                application->display_name, umi_status_text(status));
            goto reported;
        }
        goto reported;
    }

    executable = resolve_application_executable(application);
    if (executable == NULL) {
        message = g_strdup_printf(
            "Native GUI for %s was not found. Build or install it, then Refresh.",
            application->display_name);
        status = UMI_STATUS_NOT_FOUND;
        goto reported;
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
        status = UMI_STATUS_UNAVAILABLE;
        goto reported;
    }

reported:
    if (!header->destroy_pending) {
        if (message == NULL && status != UMI_STATUS_OK)
            message = g_strdup_printf("Launch request was not accepted: %s.",
                umi_status_text(status));
        set_application_row_result(header, application->application_id, status, message);
        set_application_catalogue_status(header,
            message != NULL ? message : (status == UMI_STATUS_OK
                ? "Launch request accepted; startup is not yet confirmed."
                : "Launch request was not accepted."),
            status != UMI_STATUS_OK);
        if (status == UMI_STATUS_OK && !header->dispatching_applications) {
            (void)umi_application_launch_selection_set_selected(
                header->application_selection, application->application_id, false);
            sync_application_selection(header);
            if (header->application_catalogue_button != NULL &&
                gtk_widget_get_mapped(header->application_catalogue_button)) {
                gtk_menu_button_popdown(
                    GTK_MENU_BUTTON(header->application_catalogue_button));
            }
        }
    }
finished:
    g_free(message);
    g_free(executable);
    if (error != NULL) g_error_free(error);
    finish_header_operation(header);
    return status;
}

/* Adapt the existing per-application host request to the shared batch contract. */
static UmiStatus dispatch_application_request(const char *application_id, void *context)
{
    UmiGtk4WorkstationShellHeader *header = context;
    const UmiApplicationDefinition *application;

    if (header == NULL || header->destroy_pending) return UMI_STATUS_CANCELLED;
    application = umi_application_portfolio_find(application_id);
    if (application == NULL) return UMI_STATUS_NOT_FOUND;
    return launch_portfolio_application(header, application,
        UMI_GTK4_WORKSTATION_APPLICATION_OPEN_STANDARD);
}

/* Accepted requests leave selection; failed requests remain available for retry.
 * Batch launch never marks catalogue records running without process evidence. */
UmiStatus umi_gtk4_ws_shell_header_catalogue_dispatch(
    UmiGtk4WorkstationShellHeader *header,
    UmiApplicationLaunchDispatchReport *out_report)
{
    UmiApplicationLaunchSelectionSnapshot snapshot;
    UmiStatus status;
    size_t index;
    char summary[192U];

    if (header == NULL || header->destroy_pending || out_report == NULL ||
        header->application_selection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A reentrant caller may pass the active report itself. Reject it before
     * clearing storage so it cannot erase evidence from the outer dispatch. */
    if (header->dispatching_applications) return UMI_STATUS_BUSY;
    (void)memset(out_report, 0, sizeof(*out_report));
    header->operation_depth += 1U;
    header->dispatching_applications = true;
    status = umi_application_launch_selection_snapshot(header->application_selection, &snapshot);
    if (status == UMI_STATUS_OK) {
        for (index = 0U; index < snapshot.choice_count; ++index) {
            UmiApplicationLaunchChoice choice;
            if (umi_application_launch_selection_at(
                    header->application_selection, index, &choice) == UMI_STATUS_OK &&
                choice.selected) {
                set_application_row_result(header, choice.application_id, UMI_STATUS_OK, "");
            }
        }
        sync_application_selection(header);
        status = umi_application_launch_selection_dispatch(
            header->application_selection, dispatch_application_request, header, out_report);
    }
    if (!header->destroy_pending) {
        for (index = 0U; index < out_report->result_count; ++index) {
            const UmiApplicationLaunchDispatchResult *result = &out_report->results[index];
            GtkWidget *item = find_application_catalogue_item(header, result->application_id);
            GtkWidget *label = item != NULL
                ? g_object_get_data(G_OBJECT(item), "umicom-application-result") : NULL;
            if (label != NULL && gtk_label_get_text(GTK_LABEL(label))[0] == '\0')
                set_application_row_result(header, result->application_id, result->status, NULL);
        }
        if (out_report->result_count == 0U) {
            set_application_catalogue_status(header, "Select applications to open.", false);
        } else {
            (void)snprintf(summary, sizeof(summary),
                "%zu launch request%s accepted; %zu failed.%s",
                out_report->accepted_count, out_report->accepted_count == 1U ? "" : "s",
                out_report->failed_count,
                out_report->failed_count > 0U ? " Failed requests remain selected for retry." : "");
            set_application_catalogue_status(header, summary, out_report->failed_count > 0U);
        }
    }
    header->dispatching_applications = false;
    sync_application_selection(header);
    finish_header_operation(header);
    return status;
}

/* Apply a checkbox edit through the same shared command used by native tests. */
static void on_application_selected_toggled(GtkCheckButton *button, gpointer user_data)
{
    UmiGtk4WorkstationShellHeader *header = user_data;
    const char *id = g_object_get_data(G_OBJECT(button), "umicom-application-id");

    if (header == NULL || header->destroy_pending || header->syncing_application_selection)
        return;
    (void)umi_gtk4_ws_shell_header_catalogue_set_selected(
        header, id, gtk_check_button_get_active(button));
}

/* Dispatch selected requests with portfolio-sized result storage off the GTK stack. */
static void on_open_selected_applications_clicked(GtkButton *button, gpointer user_data)
{
    UmiApplicationLaunchDispatchReport *report = calloc(1U, sizeof(*report));
    (void)button;
    if (report == NULL) {
        set_application_catalogue_status(user_data,
            "Unable to allocate launch results. Please try again.", true);
        return;
    }
    (void)umi_gtk4_ws_shell_header_catalogue_dispatch(user_data, report);
    free(report);
}

/* Clear explicit choices without changing discovery, filtering or request results. */
static void on_clear_application_selection_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4WorkstationShellHeader *header = user_data;
    (void)button;

    if (header == NULL || header->destroy_pending || header->dispatching_applications) return;
    header->operation_depth += 1U;
    (void)umi_application_launch_selection_clear(header->application_selection);
    sync_application_selection(header);
    finish_header_operation(header);
}

/* Recheck native GUI or delegated-host availability through the public command. */
static void on_refresh_applications_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    (void)umi_gtk4_ws_shell_header_catalogue_refresh(user_data);
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

    if (header == NULL || header->destroy_pending ||
        header->dispatching_applications || button == NULL) return;
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
    if (header == NULL || header->destroy_pending || header->dispatching_applications) return;
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
    GtkWidget *item;
    GtkWidget *check;
    GtkWidget *button;
    GtkWidget *content;
    GtkWidget *title;
    GtkWidget *purpose;
    GtkWidget *availability;
    GtkWidget *result;
    char *search_text;
    char *automation_id;

    if (header == NULL || application == NULL ||
        application->application_id == NULL ||
        application->display_name == NULL) {
        return NULL;
    }
    item = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    check = gtk_check_button_new();
    button = gtk_button_new_with_label("Open");
    content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    title = gtk_label_new(application->display_name);
    purpose = gtk_label_new(
        application->purpose != NULL ? application->purpose : "");
    availability = gtk_label_new("");
    result = gtk_label_new("");
    if (item == NULL || check == NULL || button == NULL || content == NULL ||
        title == NULL || purpose == NULL || availability == NULL || result == NULL) {
        return NULL;
    }

    gtk_widget_add_css_class(button, "flat");
    gtk_widget_add_css_class(item, "umicom-application-catalogue-item");
    gtk_widget_set_hexpand(content, TRUE);
    gtk_widget_set_valign(check, GTK_ALIGN_START);
    gtk_widget_set_valign(button, GTK_ALIGN_START);
    gtk_widget_set_halign(content, GTK_ALIGN_FILL);
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_label_set_xalign(GTK_LABEL(purpose), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(purpose), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(purpose), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_max_width_chars(GTK_LABEL(purpose), 46);
    gtk_label_set_xalign(GTK_LABEL(availability), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(availability), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(availability), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_max_width_chars(GTK_LABEL(availability), 46);
    gtk_label_set_xalign(GTK_LABEL(result), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(result), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(result), PANGO_WRAP_WORD_CHAR);
    gtk_label_set_max_width_chars(GTK_LABEL(result), 46);
    gtk_widget_add_css_class(title, "heading");
    gtk_widget_add_css_class(purpose, "dim-label");
    gtk_widget_add_css_class(availability, "dim-label");
    gtk_box_append(GTK_BOX(content), title);
    gtk_box_append(GTK_BOX(content), purpose);
    gtk_widget_set_visible(purpose,
        application->purpose != NULL && application->purpose[0] != '\0');
    gtk_box_append(GTK_BOX(content), availability);
    gtk_box_append(GTK_BOX(content), result);
    gtk_widget_set_visible(result, FALSE);
    gtk_box_append(GTK_BOX(item), check);
    gtk_box_append(GTK_BOX(item), content);
    gtk_box_append(GTK_BOX(item), button);

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
    g_object_set_data_full(G_OBJECT(check), "umicom-application-id",
        g_strdup(application->application_id), g_free);
    g_object_set_data_full(G_OBJECT(item), "umicom-application-id",
        g_strdup(application->application_id), g_free);
    g_object_set_data(G_OBJECT(item), "umicom-application-check", check);
    g_object_set_data(G_OBJECT(item), "umicom-application-open", button);
    g_object_set_data(G_OBJECT(item), "umicom-application-availability", availability);
    g_object_set_data(G_OBJECT(item), "umicom-application-result", result);
    g_object_set_data_full(
        G_OBJECT(item),
        "umicom-application-search",
        search_text,
        g_free);
    automation_id = g_strdup_printf(
        "workstation.application.%s", application->application_id);
    if (automation_id != NULL) {
        (void)umi_gtk4_automation_tag_widget(button, automation_id);
        g_free(automation_id);
    }
    automation_id = g_strdup_printf(
        "workstation.application.select.%s", application->application_id);
    if (automation_id != NULL) {
        (void)umi_gtk4_automation_tag_widget(check, automation_id);
        g_free(automation_id);
    }
    automation_id = g_strdup_printf(
        "workstation.application.result.%s", application->application_id);
    if (automation_id != NULL) {
        (void)umi_gtk4_automation_tag_widget(result, automation_id);
        g_free(automation_id);
    }
    automation_id = g_strdup_printf("Select %s", application->display_name);
    gtk_accessible_update_property(GTK_ACCESSIBLE(check),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        automation_id != NULL ? automation_id : application->display_name, -1);
    g_free(automation_id);
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
    g_signal_connect(check, "toggled",
        G_CALLBACK(on_application_selected_toggled), header);
    return item;
}

/* Populate the catalogue from the one canonical Framework application list. */
static UmiStatus populate_application_catalogue(
    UmiGtk4WorkstationShellHeader *header)
{
    size_t index;
    size_t added = 0U;
    UmiApplicationLaunchSelectionSnapshot snapshot;
    UmiStatus status;

    if (header == NULL || header->application_catalogue_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_launch_selection_snapshot(header->application_selection, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < snapshot.choice_count; ++index) {
        UmiApplicationLaunchChoice choice;
        const UmiApplicationDefinition *application;
        GtkWidget *item;

        status = umi_application_launch_selection_at(header->application_selection, index, &choice);
        if (status != UMI_STATUS_OK) return status;
        application = umi_application_portfolio_find(choice.application_id);
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
    GtkWidget *footer;
    UmiStatus status;

    if (header == NULL || header->root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_runtime_catalogue_create(&header->application_runtime_catalogue);
    if (status == UMI_STATUS_OK)
        status = umi_application_runtime_catalogue_seed_portfolio(header->application_runtime_catalogue);
    if (status == UMI_STATUS_OK) status = refresh_application_presence(header);
    if (status == UMI_STATUS_OK)
        status = umi_application_launch_selection_create_for_host(
            header->application_runtime_catalogue,
            umi_application_portfolio_find(header->state.application_id) != NULL
                ? header->state.application_id : NULL,
            &header->application_selection);
    if (status != UMI_STATUS_OK) return status;
    header->application_catalogue_button = gtk_menu_button_new();
    header->application_catalogue_popover = gtk_popover_new();
    header->application_catalogue_search = gtk_search_entry_new();
    header->application_catalogue_list = gtk_list_box_new();
    header->application_catalogue_status = gtk_label_new("");
    header->application_selected_count = gtk_label_new("0 selected");
    header->application_open_selected = gtk_button_new_with_label("Open selected");
    header->application_clear_selection = gtk_button_new_with_label("Clear");
    header->application_refresh = gtk_button_new_with_label("Refresh");
    header->new_window_button = gtk_button_new_from_icon_name(
        "window-new-symbolic");
    header->close_button = gtk_button_new_from_icon_name(
        "window-close-symbolic");
    catalogue_content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    catalogue_title = gtk_label_new("Umicom Applications");
    scroll = gtk_scrolled_window_new();
    footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    if (header->application_catalogue_button == NULL ||
        header->application_catalogue_popover == NULL ||
        header->application_catalogue_search == NULL ||
        header->application_catalogue_list == NULL ||
        header->application_catalogue_status == NULL ||
        header->application_selected_count == NULL ||
        header->application_open_selected == NULL ||
        header->application_clear_selection == NULL || header->application_refresh == NULL ||
        header->new_window_button == NULL || header->close_button == NULL ||
        catalogue_content == NULL || catalogue_title == NULL || scroll == NULL || footer == NULL) {
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
    gtk_widget_add_css_class(header->application_open_selected, "suggested-action");
    gtk_label_set_xalign(GTK_LABEL(header->application_selected_count), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(header->application_selected_count), TRUE);
    gtk_widget_set_tooltip_text(header->application_clear_selection,
        "Clear all selected applications, including hidden search results");
    gtk_widget_set_tooltip_text(header->application_refresh,
        "Refresh native GUI availability and shared launch eligibility");
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
    gtk_box_append(GTK_BOX(catalogue_content), header->application_selected_count);
    gtk_box_append(GTK_BOX(footer), header->application_open_selected);
    gtk_box_append(GTK_BOX(footer), header->application_clear_selection);
    gtk_box_append(GTK_BOX(footer), header->application_refresh);
    gtk_box_append(GTK_BOX(catalogue_content), footer);
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
    g_signal_connect(header->application_open_selected, "clicked",
        G_CALLBACK(on_open_selected_applications_clicked), header);
    g_signal_connect(header->application_clear_selection, "clicked",
        G_CALLBACK(on_clear_application_selection_clicked), header);
    g_signal_connect(header->application_refresh, "clicked",
        G_CALLBACK(on_refresh_applications_clicked), header);
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
    (void)umi_gtk4_automation_tag_widget(header->application_selected_count,
        "workstation.application.selected-count");
    (void)umi_gtk4_automation_tag_widget(header->application_open_selected,
        "workstation.application.open-selected");
    (void)umi_gtk4_automation_tag_widget(header->application_clear_selection,
        "workstation.application.clear");
    (void)umi_gtk4_automation_tag_widget(header->application_refresh,
        "workstation.application.refresh");
    (void)umi_gtk4_automation_tag_widget(
        header->new_window_button,
        "workstation.application.new-window");
    (void)umi_gtk4_automation_tag_widget(
        header->close_button,
        "workstation.application.close");

    status = populate_application_catalogue(header);
    if (status == UMI_STATUS_OK) sync_application_selection(header);
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

/* The managed controller owns one root reference, independently of GTK parents.
 * Disconnecting first also makes externally retained child widgets harmless. */
static void release_header_storage(UmiGtk4WorkstationShellHeader *header)
{
    umi_application_launch_selection_destroy(header->application_selection);
    umi_application_runtime_catalogue_destroy(header->application_runtime_catalogue);
    g_free(header->resource_root);
    if (header->root != NULL) g_object_unref(header->root);
    free(header);
}

/* Defer final release across nested synchronous host calls and batch dispatch. */
static void finish_header_operation(UmiGtk4WorkstationShellHeader *header)
{
    if (header->operation_depth > 0U) header->operation_depth -= 1U;
    if (header->operation_depth == 0U && header->destroy_pending)
        release_header_storage(header);
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
    if (header->root != NULL) g_object_ref_sink(header->root);
    header->application_tab = gtk_box_new(
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
    if (header->root == NULL || header->application_tab == NULL ||
        header->icon == NULL ||
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
    gtk_widget_add_css_class(header->title, "umicom-workstation-identity-title");
    gtk_widget_add_css_class(header->subtitle, "dim-label");
    gtk_widget_add_css_class(header->badge, "umicom-mode-badge");
    /* Stable identities let native acceptance tests inspect the actual mark
     * and labels without relying on translated captions or screen positions. */
    (void)umi_gtk4_automation_tag_widget(header->icon, "workstation.identity.icon");
    (void)umi_gtk4_automation_tag_widget(header->title, "workstation.identity.title");
    (void)umi_gtk4_automation_tag_widget(header->subtitle, "workstation.identity.subtitle");
    (void)umi_gtk4_automation_tag_widget(header->badge, "workstation.identity.mode");
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
    /* Identity is an inline title, not a tall card. Centre each child so a
     * neighbouring control cannot stretch the mark or the mode badge. */
    gtk_widget_set_valign(header->root, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(header->icon, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(titles, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(header->title, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(header->badge, GTK_ALIGN_CENTER);
    gtk_label_set_xalign(GTK_LABEL(header->title), 0.0F);
    gtk_label_set_single_line_mode(GTK_LABEL(header->title), TRUE);
    gtk_label_set_ellipsize(GTK_LABEL(header->title), PANGO_ELLIPSIZE_END);
    gtk_label_set_max_width_chars(GTK_LABEL(header->title), 32);
    gtk_label_set_single_line_mode(GTK_LABEL(header->badge), TRUE);
    gtk_label_set_xalign(GTK_LABEL(header->subtitle), 0.0F);
    gtk_label_set_ellipsize(
        GTK_LABEL(header->subtitle), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(titles, TRUE);
    gtk_widget_set_hexpand(header->application_tab, TRUE);
    gtk_widget_set_halign(header->application_tab, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(titles), header->title);
    gtk_box_append(GTK_BOX(titles), header->subtitle);
    gtk_box_append(GTK_BOX(header->application_tab), header->icon);
    gtk_box_append(GTK_BOX(header->application_tab), titles);
    gtk_box_append(GTK_BOX(header->application_tab), header->badge);
    gtk_box_append(GTK_BOX(header->root), header->application_tab);

    status = create_application_controls(header);
    if (status != UMI_STATUS_OK) goto fail;
    gtk_widget_set_valign(header->application_catalogue_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(header->new_window_button, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(header->close_button, GTK_ALIGN_CENTER);
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
    g_signal_connect(header->icon, "notify::scale-factor",
        G_CALLBACK(on_header_icon_scale_changed), header);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(header->root),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        header->state.title,
        -1);
    *out_header = header;
    return UMI_STATUS_OK;

fail:
    {
        GtkWidget *widgets[] = {
            header->application_tab, header->icon, titles, header->title,
            header->subtitle, header->badge, header->application_catalogue_button,
            header->application_catalogue_popover, header->application_catalogue_search,
            header->application_catalogue_list, header->application_catalogue_status,
            header->application_selected_count, header->application_open_selected,
            header->application_clear_selection, header->application_refresh,
            header->new_window_button, header->close_button
        };
        GtkWidget *orphans[sizeof(widgets) / sizeof(widgets[0])];
        size_t orphan_count = 0U;
        size_t index;

        /* Inspect every parent before releasing any tree: destroying a parent
         * first would leave dangling child pointers in an allocation-failure path. */
        for (index = 0U; index < sizeof(widgets) / sizeof(widgets[0]); ++index) {
            if (widgets[index] != NULL && gtk_widget_get_parent(widgets[index]) == NULL)
                orphans[orphan_count++] = widgets[index];
        }
        disconnect_header_callbacks(header->root, header);
        for (index = 0U; index < orphan_count; ++index) {
            disconnect_header_callbacks(orphans[index], header);
            if (g_object_is_floating(orphans[index])) g_object_ref_sink(orphans[index]);
            g_object_unref(orphans[index]);
        }
        release_header_storage(header);
    }
    return status;
}

/* Invalidate callbacks immediately; active host operations defer final storage
 * release until they have produced their reports and unwound safely. */
void umi_gtk4_ws_shell_header_destroy(
    UmiGtk4WorkstationShellHeader *header)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (header == NULL || header->destroy_pending) return;
    header->destroy_pending = true;
    disconnect_header_callbacks(
        header->application_catalogue_popover, header);
    disconnect_header_callbacks(header->root, header);
    header->application_open_handler = NULL;
    header->application_open_user_data = NULL;
    if (header->operation_depth == 0U) release_header_storage(header);
}

/* Return the borrowed root for normal GTK composition. */
GtkWidget *umi_gtk4_ws_shell_header_widget(
    UmiGtk4WorkstationShellHeader *header)
{
    return header != NULL ? header->root : NULL;
}

/* Apply the appearance's contrast-aware SVG. A missing packaged asset leaves
 * the image hidden so a fake text mark can never be mistaken for the official
 * Umicom identity; packaging diagnostics are responsible for reporting it. */
UmiStatus umi_gtk4_ws_shell_header_apply_appearance(
    UmiGtk4WorkstationShellHeader *header,
    const UmiUiAppearanceProfile *profile)
{
    char reason[192U];
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

    /* Resolve only after profile validation, then retain a bounded SVG
     * paintable rather than the artwork's full-size natural dimensions. */
    update_header_icon(header);
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
    char *tooltip;
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
    /* Compact bars show one title line. The layout selector already names the
     * workspace; its copied subtitle remains discoverable by tooltip and by
     * assistive technology without doubling the header's height. */
    tooltip = header->state.subtitle[0] != '\0'
        ? g_strdup_printf("%s\n%s", header->state.title, header->state.subtitle)
        : g_strdup(header->state.title);
    gtk_widget_set_tooltip_text(
        header->application_tab, tooltip);
    g_free(tooltip);
    update_optional_label(header->subtitle, header->state.subtitle);
    if (header->state.compact) gtk_widget_set_visible(header->subtitle, FALSE);
    update_optional_label(header->badge, header->state.mode_badge);
    gtk_accessible_update_property(
        GTK_ACCESSIBLE(header->root),
        GTK_ACCESSIBLE_PROPERTY_LABEL,
        header->state.title,
        GTK_ACCESSIBLE_PROPERTY_DESCRIPTION,
        header->state.subtitle,
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
    if (header == NULL || header->destroy_pending) return UMI_STATUS_INVALID_ARGUMENT;
    if (header->dispatching_applications) return UMI_STATUS_BUSY;
    header->application_open_handler = handler;
    header->application_open_user_data = user_data;
    header->state.revision += 1U;
    return umi_gtk4_ws_shell_header_catalogue_refresh(header);
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

/* Preserve the original floating-widget transfer for the label-only API. */
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
    /* This private, unparented root has not escaped to a caller. Transfer one
     * reference out of the controller, then restore normal GTK floating ownership. */
    g_object_ref(widget);
    umi_gtk4_ws_shell_header_destroy(header);
    g_object_force_floating(G_OBJECT(widget));
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

/* Resolve and present the packaged mark. If packaging omitted the official
 * resource, keep the image hidden and let conformance diagnostics report the
 * defect instead of presenting a misleading substitute. */
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
        splash->state.icon_visible = 1;
        g_free(resolved);
    } else {
        gtk_picture_set_paintable(GTK_PICTURE(splash->icon), NULL);
        gtk_widget_set_visible(splash->icon, FALSE);
        splash->state.icon_visible = 0;
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
        splash->title == NULL ||
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
