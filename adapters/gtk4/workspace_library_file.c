/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workspace_library_file.c
 * PURPOSE: Keep explicit library creation separate from opening existing data.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workspace_library.h"

/* GIO preserves native Unicode filenames on Windows and Unix. Mapping errors
 * here lets every creative and AI host explain the same failure consistently. */
static UmiStatus FileStatus(const GError *error)
{
    if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_EXISTS)) return UMI_STATUS_ALREADY_EXISTS;
    if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND)) return UMI_STATUS_NOT_FOUND;
    if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED)) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}

UmiStatus UmiWorkspaceLibraryOpenData(const char *path, bool create, UmiDataServer **outServer)
{
    if (outServer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outServer = NULL;
    if (path == NULL || !g_utf8_validate(path, -1, NULL) || !g_path_is_absolute(path))
        return UMI_STATUS_INVALID_ARGUMENT;
    GFile *file = g_file_new_for_path(path);
    GError *error = NULL;
    UmiStatus status = UMI_STATUS_OK;
    if (create) {
        /* Exclusive creation is deliberate: a Save dialog's overwrite prompt
         * must never turn Create library into replacement of somebody's work. */
        GFileOutputStream *stream = g_file_create(file, G_FILE_CREATE_PRIVATE, NULL, &error);
        if (stream == NULL) status = FileStatus(error);
        else {
            if (!g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, &error)) status = FileStatus(error);
            g_object_unref(stream);
        }
    } else {
        /* Do not let SQLite's create-if-missing behaviour hide a mistyped path.
         * Reject directories and leaf links before lending the name to SQLite. */
        GFileInfo *info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_TYPE,
            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, &error);
        if (info == NULL) status = FileStatus(error);
        else {
            if (g_file_info_get_file_type(info) != G_FILE_TYPE_REGULAR) status = UMI_STATUS_INVALID_ARGUMENT;
            g_object_unref(info);
        }
    }
    g_clear_error(&error);
    g_object_unref(file);
    if (status == UMI_STATUS_OK) status = umi_data_server_create_sqlite(path, outServer);
    return status;
}
