/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/asset_library_file.c
 * PURPOSE: Stream creative libraries to exclusive new files and reopen bounded complete bundles on workers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "asset_library_wire.h"
#include "umicom/platform/input_file.h"
#include <string.h>
static UmiStatus Write(void *context, const void *bytes, size_t count)
{
    return UmiOutputFileWrite(context, bytes, count);
}
/* The existing output-file owner provides exclusive creation and a close
 * receipt. Keep a failed new file for diagnosis; never delete a path that a
 * caller or another process may now be inspecting. */
UmiStatus UmiCreativeAssetLibrarySaveNew(const UmiCreativeAssetLibrary *library, const char *path,
                                         const UmiCancellationToken *cancel, UmiCreativeAssetWriteResult *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    UmiStatus status = UmiOutputFileValidatePath(path);
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiCreativeLibraryArchiveSize(library, &size);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    UmiOutputFile *file = NULL;
    status = UmiOutputFileCreate(path, &file);
    if (status != UMI_STATUS_OK)
        return status;
    out->created = true;
    status = UmiCreativeLibraryArchiveWrite(library, cancel, Write, file);
    UmiStatus closed = UmiOutputFileClose(file);
    if (status == UMI_STATUS_OK)
        status = closed;
    UmiStatus inspected = UmiOutputFileRead(file, &out->file);
    if (status == UMI_STATUS_OK)
        status = inspected;
    if (status == UMI_STATUS_OK && out->file.bytes_written != (uint64_t)size)
        status = UMI_STATUS_IO_ERROR;
    UmiOutputFileDestroy(file);
    return status;
}
UmiStatus UmiCreativeAssetLibraryLoad(const char *path, size_t maximum_bytes,
                                      const UmiCancellationToken *cancel, UmiCreativeAssetLibrary **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (maximum_bytes > UMI_CREATIVE_LIBRARY_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK)
        return status;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    size_t limit = maximum_bytes + (UMI_CREATIVE_LIBRARY_ARCHIVE_MAX_BYTES - UMI_CREATIVE_LIBRARY_MAX_BYTES);
    status = UmiInputFileRead(path, limit, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetLibraryArchiveDecode(bytes, size, maximum_bytes, cancel, out);
    UmiInputFileFree(bytes);
    return status;
}
