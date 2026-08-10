#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/atomic_file.c
 *
 * PURPOSE:
 *   Implement same-directory temporary writes, flush-to-storage and atomic
 *   destination replacement on Windows and POSIX systems.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/atomic_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <process.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

static unsigned long umi_atomic_process_id(void)
{
#ifdef _WIN32
    return (unsigned long)_getpid();
#else
    return (unsigned long)getpid();
#endif
}

static UmiStatus umi_atomic_flush(FILE *file)
{
    if (fflush(file) != 0) {
        return UMI_STATUS_IO_ERROR;
    }
#ifdef _WIN32
    return _commit(_fileno(file)) == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
#else
    return fsync(fileno(file)) == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
#endif
}

static UmiStatus umi_atomic_replace(const char *temporary,
                                    const char *destination)
{
#ifdef _WIN32
    return MoveFileExA(temporary,
                       destination,
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
#else
    return rename(temporary, destination) == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
#endif
}

UmiStatus umi_atomic_file_write(const char *path,
                                const void *data,
                                size_t size)
{
    static unsigned long sequence = 0UL;
    char temporary[UMI_PATH_CAPACITY];
    char parent[UMI_PATH_CAPACITY];
    FILE *file;
    UmiStatus status;
    int written;

    if (path == NULL || path[0] == '\0' ||
        (data == NULL && size > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_fs_parent(parent, sizeof(parent), path);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (parent[0] != '\0') {
        status = umi_fs_make_directories(parent);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    sequence += 1UL;
    written = snprintf(temporary,
                       sizeof(temporary),
                       "%s.umi-tmp-%lu-%lu",
                       path,
                       umi_atomic_process_id(),
                       sequence);
    if (written < 0 || (size_t)written >= sizeof(temporary)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    file = fopen(temporary, "wb");
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    if (size > 0U && fwrite(data, 1U, size, file) != size) {
        (void)fclose(file);
        (void)remove(temporary);
        return UMI_STATUS_IO_ERROR;
    }

    status = umi_atomic_flush(file);
    if (fclose(file) != 0 && status == UMI_STATUS_OK) {
        status = UMI_STATUS_IO_ERROR;
    }
    if (status != UMI_STATUS_OK) {
        (void)remove(temporary);
        return status;
    }

    status = umi_atomic_replace(temporary, path);
    if (status != UMI_STATUS_OK) {
        (void)remove(temporary);
    }
    return status;
}

UmiStatus umi_atomic_file_write_text(const char *path,
                                     const char *text)
{
    if (text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_atomic_file_write(path, text, strlen(text));
}
