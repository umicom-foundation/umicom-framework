/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/workspace_storage_gtk4.c
 *
 * PURPOSE:
 *   Locate a canonical application's user configuration directory and open the
 *   existing SQLite Data Server only when its native host explicitly requests it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/workspace_storage.h"

#include <gio/gio.h>
#include <glib/gstdio.h>
#include <string.h>
#include <stdio.h>
#include "umicom/security/local_profile.h"

/* Application IDs are one safe path component; toolkit validation excludes
 * path separators, empty segments and traversal. No memory fallback is allowed. */
UmiStatus umi_gtk4_workspace_storage_open(
    const char *canonical_application_id, UmiDataServer **out_server)
{
    char *directory;
    char *path;
    UmiStatus status;
    if (out_server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_server = NULL;
    if (canonical_application_id == NULL ||
        !g_application_id_is_valid(canonical_application_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    directory = g_build_filename(g_get_user_config_dir(), "umicom",
        canonical_application_id, NULL);
    path = g_build_filename(directory, "workspace-layouts.sqlite3", NULL);
    /* The existing Data Server publishes a bounded native path in snapshots. */
    if (strlen(path) >= sizeof(((UmiDataServerSnapshot *)0)->path)) {
        g_free(path);
        g_free(directory);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = g_mkdir_with_parents(directory, 0700) == 0
        ? umi_data_server_create_sqlite(path, out_server) : UMI_STATUS_IO_ERROR;
    g_free(path);
    g_free(directory);
    return status;
}

/* The same canonical name selects the same directory across application
 * restarts; separators and traversal are rejected before joining any path. */
UmiStatus UmiGtk4WorkspaceProfileStorageId(const char *application_id, const char *profile,
                                         char *out, size_t capacity)
{
    char canonical[UMI_LOCAL_PROFILE_NAME_CAPACITY], candidate[256];
    if (application_id == NULL || out == NULL || capacity == 0U ||
        !g_application_id_is_valid(application_id) || UmiLocalProfileName(profile,canonical) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    int n = snprintf(candidate,sizeof(candidate),"%s.profile.user_%s",application_id,canonical);
    if (n < 0 || (size_t)n >= sizeof(candidate) || (size_t)n >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out,candidate,(size_t)n+1U); return UMI_STATUS_OK;
}
