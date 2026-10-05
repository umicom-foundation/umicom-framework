/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/local_replace.c
 * PURPOSE: Publish fully written local settings through native same-directory replacement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/local_replace.h"
#include "umicom/platform/output_file.h"
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
/* Name uniqueness is confirmed by exclusive file creation, not by the counter.
 * Atomic increments avoid a C data race between independent settings workers. */
static atomic_ulong replaceSequence = 0;
#ifdef _WIN32
static UmiStatus ReplaceWide(const char *path, wchar_t *out)
{
    return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, out, (int)UMI_PATH_CAPACITY) != 0
               ? UMI_STATUS_OK
               : UMI_STATUS_INVALID_ARGUMENT;
}
static UmiStatus ReplaceDestination(const char *path)
{
    wchar_t wide[UMI_PATH_CAPACITY];
    UmiStatus status = ReplaceWide(path, wide);
    if (status != UMI_STATUS_OK)
        return status;
    DWORD attributes = GetFileAttributesW(wide);
    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND   ? UMI_STATUS_OK
               : error == ERROR_PATH_NOT_FOUND ? UMI_STATUS_NOT_FOUND
                                               : UMI_STATUS_IO_ERROR;
    }
    return (attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DEVICE)) ==
                   0U
               ? UMI_STATUS_OK
               : UMI_STATUS_INVALID_ARGUMENT;
}
static void ReplaceRemove(const char *path)
{
    wchar_t wide[UMI_PATH_CAPACITY];
    if (ReplaceWide(path, wide) == UMI_STATUS_OK)
        (void)DeleteFileW(wide);
}
static UmiStatus ReplacePublish(const char *from, const char *to)
{
    wchar_t source[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
    if (ReplaceWide(from, source) != UMI_STATUS_OK || ReplaceWide(to, destination) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    return MoveFileExW(source, destination, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)
               ? UMI_STATUS_OK
               : UMI_STATUS_IO_ERROR;
}
#else
static UmiStatus ReplaceDestination(const char *path)
{
    struct stat info;
    if (lstat(path, &info) != 0)
        return errno == ENOENT ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    return S_ISREG(info.st_mode) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static void ReplaceRemove(const char *path) { (void)unlink(path); }
static UmiStatus ReplacePublish(const char *from, const char *to)
{
    return rename(from, to) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}
#endif
UmiStatus UmiLocalFileReplace(const char *path, const void *bytes, size_t size)
{
    if (bytes == NULL && size != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status == UMI_STATUS_OK)
        status = ReplaceDestination(path);
    if (status != UMI_STATUS_OK)
        return status;
    char temporary[UMI_PATH_CAPACITY];
    UmiOutputFile *file = NULL;
#ifdef _WIN32
    unsigned long process = (unsigned long)GetCurrentProcessId();
#else
    unsigned long process = (unsigned long)getpid();
#endif
    /* A bounded retry handles abandoned files and overlapping process IDs.
     * Never truncate a candidate: only the successful exclusive create owns it. */
    for (unsigned attempt = 0U; attempt < 64U; ++attempt)
    {
        unsigned long sequence = atomic_fetch_add_explicit(&replaceSequence, 1UL, memory_order_relaxed);
        int length = snprintf(temporary, sizeof(temporary), "%s.umi-stage-%lu-%lu", path, process, sequence);
        if (length < 0 || (size_t)length >= sizeof(temporary))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        status = UmiOutputFileCreate(temporary, &file);
        if (status != UMI_STATUS_ALREADY_EXISTS)
            break;
    }
    if (status != UMI_STATUS_OK)
        return status;
    status = UmiOutputFileWrite(file, bytes, size);
    UmiStatus closed = UmiOutputFileClose(file);
    if (status == UMI_STATUS_OK)
        status = closed;
    UmiOutputFileDestroy(file);
    /* A second check catches ordinary changes during the write. This is not
     * a lock against a hostile actor renaming entries in the parent directory. */
    if (status == UMI_STATUS_OK)
        status = ReplaceDestination(path);
    if (status == UMI_STATUS_OK)
        status = ReplacePublish(temporary, path);
    if (status != UMI_STATUS_OK)
        ReplaceRemove(temporary);
    return status;
}
