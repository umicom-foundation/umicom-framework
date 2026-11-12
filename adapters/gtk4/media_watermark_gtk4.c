/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/media_watermark_gtk4.c
 *
 * PURPOSE:
 *   Materialise canonical embedded artwork for an encoder and report cleanup failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "media_watermark_gtk4.h"
#include "media_workspace_style.h"
#include "umicom/platform/output_file.h"

struct UmiMediaWatermarkFile {
    GFile *file;
    char *path;
};

UmiStatus UmiMediaWatermarkFileCreate(UmiMediaWatermarkFile **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL) return UMI_STATUS_INVALID_STATE;
    UmiMediaWatermarkFile *owner = g_try_new0(UmiMediaWatermarkFile, 1);
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *out = owner;
    GFileIOStream *stream = NULL;
    GError *error = NULL;
    owner->file = g_file_new_tmp("umicom-watermark-XXXXXX", &stream, &error);
    if (owner->file == NULL) {
        g_clear_error(&error);
        return UMI_STATUS_IO_ERROR;
    }
    owner->path = g_file_get_path(owner->file);
    UmiStatus status = UmiOutputFileValidatePath(owner->path);
    if (status == UMI_STATUS_OK) {
        GBytes *logo = UmiMediaWorkspaceLogoBytes();
        gsize size = 0U, written = 0U;
        const void *bytes = g_bytes_get_data(logo, &size);
        gboolean complete = g_output_stream_write_all(
            g_io_stream_get_output_stream(G_IO_STREAM(stream)), bytes, size,
            &written, NULL, &error);
        if (!complete || written != size) status = UMI_STATUS_IO_ERROR;
        g_bytes_unref(logo);
        g_clear_error(&error);
    }
    /* Always close the writer before an encoder opens the SVG, including on
     * Windows. The cleanup owner remains available after every partial failure. */
    if (!g_io_stream_close(G_IO_STREAM(stream), NULL, &error) && status == UMI_STATUS_OK)
        status = UMI_STATUS_IO_ERROR;
    g_clear_error(&error);
    g_object_unref(stream);
    return status;
}

const char *UmiMediaWatermarkFilePath(const UmiMediaWatermarkFile *file)
{
    return file == NULL ? NULL : file->path;
}

UmiStatus UmiMediaWatermarkFileDestroy(UmiMediaWatermarkFile *owner)
{
    if (owner == NULL) return UMI_STATUS_OK;
    UmiStatus status = UMI_STATUS_OK;
    if (owner->file != NULL) {
        GError *error = NULL;
        if (!g_file_delete(owner->file, NULL, &error) &&
            !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
            status = UMI_STATUS_IO_ERROR;
        g_clear_error(&error);
        g_object_unref(owner->file);
    }
    g_free(owner->path);
    g_free(owner);
    return status;
}
