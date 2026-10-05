/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/output_file.h
 * PURPOSE: Write an explicitly selected new output file without overwriting existing evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_OUTPUT_FILE_H
#define UMICOM_PLATFORM_OUTPUT_FILE_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiOutputFile UmiOutputFile;
typedef struct UmiOutputFileSnapshot {
    char path[UMI_PATH_CAPACITY];
    uint64_t bytes_written;
    UmiStatus status;
    bool closed;
} UmiOutputFileSnapshot;

/* Syntax only, with no storage access. Require an absolute ordinary file path
 * and an existing, trusted parent directory at creation time. Windows accepts
 * UTF-8 drive paths; device names, alternate streams and UNC paths are refused.
 * POSIX uses native path bytes. Dot segments are refused on both platforms.
 * Parent aliases may be followed: this API is not a workspace confinement
 * boundary. Callers must choose a directory they control. */
UmiStatus UmiOutputFileValidatePath(const char *path);
/* Exclusive creation never opens an existing leaf, including a symlink. The
 * file is not inherited by child processes. No directory is created and no
 * existing or partial file is deleted. All calls are serialized by the owner. */
UmiStatus UmiOutputFileCreate(const char *path, UmiOutputFile **out_file);
/* Write raw bytes, including NUL. Retain the first I/O failure; later writes
 * return it without writing again. bytes_written counts actual accepted bytes,
 * including a partial write. Disk I/O can block the calling worker. */
UmiStatus UmiOutputFileWrite(UmiOutputFile *file, const void *bytes, size_t length);
/* Flush to the OS storage interface and close once. Repeated close is safe.
 * Success is not a guarantee against power loss or subsequent external edits. */
UmiStatus UmiOutputFileClose(UmiOutputFile *file);
UmiStatus UmiOutputFileRead(const UmiOutputFile *file, UmiOutputFileSnapshot *out_snapshot);
/* Close if necessary, release the handle, and retain the file on disk. */
void UmiOutputFileDestroy(UmiOutputFile *file);
#ifdef __cplusplus
}
#endif
#endif
