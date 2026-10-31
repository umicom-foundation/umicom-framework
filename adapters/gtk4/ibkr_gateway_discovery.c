/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_gateway_discovery.c
 *
 * PURPOSE:
 *   Discover installed Gateway launchers without reading account configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "ibkr_gateway_discovery.h"
#include <string.h>

#define GATEWAY_CANDIDATE_LIMIT 16U
#define GATEWAY_ENTRY_LIMIT 128U
#ifdef G_OS_WIN32
#define GATEWAY_LAUNCHER "ibgateway.exe"
#else
#define GATEWAY_LAUNCHER "ibgateway"
#endif

static gboolean Fail(GError **error, const char *message)
{
    g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT, message);
    return FALSE;
}

static gboolean LocalPath(const char *value)
{
    if (value == NULL || !g_utf8_validate(value, -1, NULL) ||
        strlen(value) > 4096U || !g_path_is_absolute(value) || strstr(value, "://") != NULL)
        return FALSE;
    for (const char *p = value; *p != '\0'; ++p)
        if ((guchar)*p < 32U || *p == 127) return FALSE;
#ifdef G_OS_WIN32
    /* Network shares and device paths are not installation roots. */
    if (!g_ascii_isalpha(value[0]) || value[1] != ':' ||
        (value[2] != '/' && value[2] != '\\')) return FALSE;
#endif
    return TRUE;
}

static gboolean PlainFile(GFile *file, GFileType type, GCancellable *cancel)
{
    GFileInfo *info = g_file_query_info(file,
        G_FILE_ATTRIBUTE_STANDARD_TYPE "," G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK,
        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, cancel, NULL);
    gboolean valid = info != NULL && !g_file_info_get_is_symlink(info) &&
        g_file_info_get_file_type(info) == type;
    g_clear_object(&info);
    return valid;
}

gboolean UmiIbkrGatewayValidate(const char *value, GCancellable *cancel, GError **error)
{
    if (g_cancellable_set_error_if_cancelled(cancel, error)) return FALSE;
    if (!LocalPath(value)) return Fail(error, "Choose an absolute local Gateway launcher path.");
    gchar *name = g_path_get_basename(value);
#ifdef G_OS_WIN32
    gboolean named = g_ascii_strcasecmp(name, GATEWAY_LAUNCHER) == 0;
#else
    gboolean named = strcmp(name, GATEWAY_LAUNCHER) == 0;
#endif
    g_free(name);
    if (!named) return Fail(error, "Choose the ibgateway launcher, not an installer or uninstaller.");
    GFile *file = g_file_new_for_path(value);
    gboolean valid = PlainFile(file, G_FILE_TYPE_REGULAR, cancel);
    /* Reject symbolic-link ancestors too. A selected launcher remains an
     * external program; discovery does not authenticate its publisher. */
    GFile *parent = g_file_get_parent(file);
    unsigned depth = 0U;
    while (valid && parent != NULL && depth++ < 256U) {
        valid = PlainFile(parent, G_FILE_TYPE_DIRECTORY, cancel);
        GFile *next = g_file_get_parent(parent);
        g_object_unref(parent);
        parent = next;
    }
    if (parent != NULL) valid = FALSE;
    g_clear_object(&parent);
    g_object_unref(file);
#ifndef G_OS_WIN32
    valid = valid && g_file_test(value, G_FILE_TEST_IS_EXECUTABLE);
#endif
    if (g_cancellable_set_error_if_cancelled(cancel, error)) return FALSE;
    return valid ? TRUE : Fail(error, "Gateway launcher is missing, inaccessible, or linked. Choose its installed local path.");
}

static void AddCandidate(GPtrArray *found, const char *value, GCancellable *cancel, gboolean *incomplete)
{
    if (!UmiIbkrGatewayValidate(value, cancel, NULL)) return;
    gchar *canonical = g_canonicalize_filename(value, NULL);
    for (guint i = 0U; i < found->len; ++i) {
#ifdef G_OS_WIN32
        gboolean same = g_ascii_strcasecmp(g_ptr_array_index(found, i), canonical) == 0;
#else
        gboolean same = strcmp(g_ptr_array_index(found, i), canonical) == 0;
#endif
        if (same) { g_free(canonical); return; }
    }
    if (found->len >= GATEWAY_CANDIDATE_LIMIT) {
        *incomplete = TRUE;
        g_free(canonical);
    } else g_ptr_array_add(found, canonical);
}

static void ScanRoot(GPtrArray *found, const char *root, GCancellable *cancel, gboolean *incomplete)
{
    if (!LocalPath(root)) { *incomplete = TRUE; return; }
    gchar *direct = g_build_filename(root, GATEWAY_LAUNCHER, NULL);
    AddCandidate(found, direct, cancel, incomplete);
    g_free(direct);
    GFile *directory = g_file_new_for_path(root);
    GError *error = NULL;
    GFileEnumerator *entries = g_file_enumerate_children(directory,
        G_FILE_ATTRIBUTE_STANDARD_NAME "," G_FILE_ATTRIBUTE_STANDARD_TYPE ","
        G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, cancel, &error);
    if (entries != NULL) {
        unsigned seen = 0U;
        for (;;) {
            GFileInfo *info = g_file_enumerator_next_file(entries, cancel, &error);
            if (info == NULL) break;
            if (seen++ == GATEWAY_ENTRY_LIMIT) {
                *incomplete = TRUE; g_object_unref(info); break;
            }
            const char *name = g_file_info_get_name(info);
            if (g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY &&
                !g_file_info_get_is_symlink(info)) {
                gchar *candidate = g_build_filename(root, name, GATEWAY_LAUNCHER, NULL);
                AddCandidate(found, candidate, cancel, incomplete);
                g_free(candidate);
            }
            g_object_unref(info);
        }
        g_object_unref(entries);
    }
    if (error != NULL && !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
        *incomplete = TRUE;
    g_clear_error(&error);
    g_object_unref(directory);
}

static gint ComparePaths(gconstpointer a, gconstpointer b)
{
    return g_strcmp0(*(char *const *)a, *(char *const *)b);
}

gboolean UmiIbkrGatewayDiscover(const char *const *roots, const char *custom,
    GCancellable *cancel, char ***outPaths, gboolean *outIncomplete, GError **error)
{
    if (outPaths == NULL || outIncomplete == NULL) return Fail(error, "Discovery outputs are required.");
    if (g_cancellable_set_error_if_cancelled(cancel, error)) return FALSE;
    GPtrArray *found = g_ptr_array_new_with_free_func(g_free);
    gboolean incomplete = FALSE;
    if (custom != NULL && custom[0] != '\0') {
        if (!UmiIbkrGatewayValidate(custom, cancel, error)) { g_ptr_array_unref(found); return FALSE; }
        AddCandidate(found, custom, cancel, &incomplete);
    } else if (roots != NULL) {
        unsigned i = 0U;
        for (; i < 8U && roots[i] != NULL; ++i) ScanRoot(found, roots[i], cancel, &incomplete);
        if (i == 8U && roots[i] != NULL) incomplete = TRUE;
    } else {
#ifdef G_OS_WIN32
        ScanRoot(found, "C:/Jts/ibgateway", cancel, &incomplete);
#endif
        gchar *homeRoot = g_build_filename(g_get_home_dir(), "Jts", "ibgateway", NULL);
        ScanRoot(found, homeRoot, cancel, &incomplete);
        g_free(homeRoot);
    }
    if (g_cancellable_set_error_if_cancelled(cancel, error)) { g_ptr_array_unref(found); return FALSE; }
    g_ptr_array_sort(found, ComparePaths);
    g_ptr_array_add(found, NULL);
    *outPaths = (char **)g_ptr_array_free(found, FALSE);
    *outIncomplete = incomplete;
    return TRUE;
}

gboolean UmiIbkrGatewayLaunch(const char *value, GCancellable *cancel, GError **error)
{
    if (!UmiIbkrGatewayValidate(value, cancel, error)) return FALSE;
    if (g_cancellable_set_error_if_cancelled(cancel, error)) return FALSE;
    gchar *directory = g_path_get_dirname(value);
    gchar *argv[] = {(gchar *)value, NULL};
    /* Explicit argv avoids a command shell and never appends credentials.
     * The broker's interactive login window must remain visible to its user. */
    gboolean started = g_spawn_async(directory, argv, NULL, G_SPAWN_DEFAULT,
        NULL, NULL, NULL, error);
    g_free(directory);
    return started;
}
