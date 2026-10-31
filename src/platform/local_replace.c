/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/local_replace.c
 *
 * PURPOSE:
 *   Publish owned local settings with bounded contention recovery and failure-stage diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/* The single-attempt implementation is retained for comparison. The active
 * implementation below reports cleanup and retries explicit sharing conflicts. */
#if 0
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
#endif
static bool ReplaceRemove(const char *path)
{
    wchar_t wide[UMI_PATH_CAPACITY];
    return ReplaceWide(path, wide) == UMI_STATUS_OK && DeleteFileW(wide) != 0;
}
/* Same-directory publication can briefly conflict with an ordinary reader
 * that did not share delete access. Retry only the two explicit contention
 * errors. Never change permissions, remove the destination first, or treat a
 * security denial as transient. Preserve the final native error before cleanup. */
static UmiStatus ReplacePublish(const char *from, const char *to,
                                UmiLocalFileReplaceReport *report)
{
    wchar_t source[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
    if (ReplaceWide(from, source) != UMI_STATUS_OK || ReplaceWide(to, destination) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (unsigned attempt = 0U; attempt < 8U; ++attempt)
    {
        if (attempt != 0U)
        {
            Sleep((DWORD)(attempt * 10U));
            UmiStatus checked = ReplaceDestination(to);
            if (checked != UMI_STATUS_OK)
            {
                report->stage = UMI_LOCAL_REPLACE_RECHECK;
                return checked;
            }
        }
        ++report->publish_attempts;
        if (MoveFileExW(source, destination, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            return UMI_STATUS_OK;
        DWORD error = GetLastError();
        report->last_publish_error = (uint32_t)error;
        if (error != ERROR_SHARING_VIOLATION && error != ERROR_LOCK_VIOLATION)
            break;
    }
    return UMI_STATUS_IO_ERROR;
}

#else
static UmiStatus ReplaceDestination(const char *path)
{
    struct stat info;
    if (lstat(path, &info) != 0)
        return errno == ENOENT ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
    return S_ISREG(info.st_mode) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/* Retain the earlier POSIX helpers for review; copied diagnostics below
 * preserve errno before staging cleanup, without changing rename semantics. */
#if 0
static void ReplaceRemove(const char *path) { (void)unlink(path); }
static UmiStatus ReplacePublish(const char *from, const char *to)
{
    return rename(from, to) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}
#endif
static bool ReplaceRemove(const char *path) { return unlink(path) == 0; }
static UmiStatus ReplacePublish(const char *from, const char *to,
                                UmiLocalFileReplaceReport *report)
{
    ++report->publish_attempts;
    if (rename(from, to) == 0)
        return UMI_STATUS_OK;
    report->last_publish_error = (uint32_t)errno;
    return UMI_STATUS_IO_ERROR;
}
#endif
/* Preserve the original operation for ownership review. The detailed entry
 * point below retains staging and validation while making failures observable. */
#if 0
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
#endif

/* Keep the existing entry point while centralising diagnostics in one operation.
 * Callers without a report receive identical validation, ownership and cleanup
 * rules. A report is a copied result, never shared process-wide error state. */
UmiStatus UmiLocalFileReplaceDetailed(const char *path, const void *bytes, size_t size,
                                     UmiLocalFileReplaceReport *out_report)
{
    UmiLocalFileReplaceReport report = {0};
    report.stage = UMI_LOCAL_REPLACE_VALIDATE;
    UmiStatus status = (bytes == NULL && size != 0U) ? UMI_STATUS_INVALID_ARGUMENT
                                                   : UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK)
        goto finished;
    report.stage = UMI_LOCAL_REPLACE_DESTINATION;
    status = ReplaceDestination(path);
    if (status != UMI_STATUS_OK)
        goto finished;
    char temporary[UMI_PATH_CAPACITY];
    UmiOutputFile *file = NULL;
#ifdef _WIN32
    unsigned long process = (unsigned long)GetCurrentProcessId();
#else
    unsigned long process = (unsigned long)getpid();
#endif
    report.stage = UMI_LOCAL_REPLACE_CREATE;
    /* Only an exclusive create establishes ownership. An abandoned candidate
     * belongs to someone else and is never truncated or removed by this call. */
    for (unsigned attempt = 0U; attempt < 64U; ++attempt)
    {
        unsigned long sequence = atomic_fetch_add_explicit(&replaceSequence, 1UL, memory_order_relaxed);
        int length = snprintf(temporary, sizeof(temporary), "%s.umi-stage-%lu-%lu", path, process, sequence);
        if (length < 0 || (size_t)length >= sizeof(temporary))
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto finished;
        }
        status = UmiOutputFileCreate(temporary, &file);
        if (status != UMI_STATUS_ALREADY_EXISTS)
            break;
    }
    if (status != UMI_STATUS_OK)
        goto finished;
    report.stage = UMI_LOCAL_REPLACE_WRITE;
    status = UmiOutputFileWrite(file, bytes, size);
    UmiStatus closed = UmiOutputFileClose(file);
    if (status == UMI_STATUS_OK)
    {
        report.stage = UMI_LOCAL_REPLACE_CLOSE;
        status = closed;
    }
    UmiOutputFileDestroy(file);
    if (status == UMI_STATUS_OK)
    {
        report.stage = UMI_LOCAL_REPLACE_RECHECK;
        status = ReplaceDestination(path);
    }
    if (status == UMI_STATUS_OK)
    {
        report.stage = UMI_LOCAL_REPLACE_PUBLISH;
        status = ReplacePublish(temporary, path, &report);
    }
    if (status != UMI_STATUS_OK)
    {
        report.cleanup_attempted = true;
        report.cleanup_succeeded = ReplaceRemove(temporary);
    }
    else
        report.stage = UMI_LOCAL_REPLACE_COMPLETE;
finished:
    report.status = status;
    if (out_report != NULL)
        *out_report = report;
    return status;
}

UmiStatus UmiLocalFileReplace(const char *path, const void *bytes, size_t size)
{
    return UmiLocalFileReplaceDetailed(path, bytes, size, NULL);
}
