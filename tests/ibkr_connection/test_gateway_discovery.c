/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_gateway_discovery.c
 *
 * PURPOSE:
 *   Verify Gateway discovery and native ownership without launching a broker.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/ibkr_gateway_discovery.h"
#include <glib/gstdio.h>
#include <string.h>
#ifdef G_OS_WIN32
#define LAUNCHER "ibgateway.exe"
#else
#define LAUNCHER "ibgateway"
#endif
#define CHECK(x) do { if (!(x)) { g_printerr("Check failed at %d: %s\n", __LINE__, #x); return 1; } } while (0)

static int Run(const char *mode, const char *root, GPtrArray *created)
{
    char **paths = NULL;
    gboolean incomplete = FALSE;
    GError *error = NULL;
    const char *roots[] = {root, NULL};
    gchar *version = g_build_filename(root, "1051", NULL);
    g_ptr_array_add(created, version);
    CHECK(g_mkdir(version, 0700) == 0);
    const char *directory = !strcmp(mode, "direct") ? root : version;
    unsigned total = !strcmp(mode, "limit") ? 18U : 1U;
    for (unsigned i = 0U; i < total; ++i) {
        gchar *extra = NULL;
        if (i != 0U) {
            gchar *number = g_strdup_printf("version-%u", i);
            extra = g_build_filename(root, number, NULL); g_free(number);
            g_ptr_array_add(created, extra); CHECK(g_mkdir(extra, 0700) == 0);
        }
        gchar *file = g_build_filename(extra != NULL ? extra : directory, LAUNCHER, NULL);
        g_ptr_array_add(created, file);
        CHECK(g_file_set_contents(file, "fixture; never execute", -1, NULL));
#ifndef G_OS_WIN32
        CHECK(g_chmod(file, 0700) == 0);
#endif
    }
    gchar *launcher = g_build_filename(directory, LAUNCHER, NULL);
    g_ptr_array_add(created, launcher);
    if (!strcmp(mode, "cancel")) {
        GCancellable *cancel = g_cancellable_new();
        g_cancellable_cancel(cancel);
        CHECK(!UmiIbkrGatewayDiscover(roots, "", cancel, &paths, &incomplete, &error));
        CHECK(g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED) && paths == NULL);
        g_clear_error(&error); g_object_unref(cancel); return 0;
    }
    if (!strcmp(mode, "relative") || !strcmp(mode, "uninstaller") ||
        !strcmp(mode, "uri") || !strcmp(mode, "missing-custom")) {
        gchar *invalid = !strcmp(mode, "relative") ? g_strdup(LAUNCHER)
            : !strcmp(mode, "uri") ? g_strdup("https://example.invalid/ibgateway.exe")
            : g_build_filename(root, !strcmp(mode, "uninstaller") ? "uninstall.exe" : "absent/ibgateway.exe", NULL);
        CHECK(!UmiIbkrGatewayDiscover(roots, invalid, NULL, &paths, &incomplete, &error));
        CHECK(paths == NULL && error != NULL);
        g_clear_error(&error); g_free(invalid); return 0;
    }
    if (!strcmp(mode, "missing")) {
        const char *empty[] = {NULL};
        CHECK(UmiIbkrGatewayDiscover(empty, "", NULL, &paths, &incomplete, &error));
        CHECK(paths[0] == NULL && !incomplete); g_strfreev(paths); return 0;
    }
    const char *duplicates[] = {root, root, NULL};
    CHECK(UmiIbkrGatewayDiscover(!strcmp(mode, "duplicates") ? duplicates : roots,
        !strcmp(mode, "custom") ? launcher : "", NULL, &paths, &incomplete, &error));
    CHECK(error == NULL && g_strv_length(paths) == (!strcmp(mode, "limit") ? 16U : 1U));
    CHECK(incomplete == (!strcmp(mode, "limit")));
    if (strcmp(mode, "limit")) {
        gchar *expected = g_canonicalize_filename(launcher, NULL);
        CHECK(!strcmp(paths[0], expected)); g_free(expected);
    }
    g_strfreev(paths); return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    gchar *root = g_dir_make_tmp("umicom-gateway space-XXXXXX", NULL);
    if (root == NULL) return 1;
    GPtrArray *created = g_ptr_array_new_with_free_func(g_free);
    int result = Run(argv[1], root, created);
    /* Delete only fixture-owned paths, children first; never scan real installs. */
    for (guint i = created->len; i > 0U; --i) {
        const char *item = g_ptr_array_index(created, i - 1U);
        if (g_file_test(item, G_FILE_TEST_IS_DIR)) (void)g_rmdir(item);
        else (void)g_remove(item);
    }
    g_ptr_array_unref(created); (void)g_rmdir(root); g_free(root); return result;
}
