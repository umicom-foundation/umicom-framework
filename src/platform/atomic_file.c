/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/atomic_file.c
 *
 * PURPOSE:
 *   Implement the atomic file behavior for
 *   Umicom Framework.
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

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/atomic_file.c
 *
 * PURPOSE:
 *   Implement same-directory temporary writes, flush-to-storage and atomic
 *   destination replacement on Windows and POSIX systems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/atomic_file.h"

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdatomic.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <process.h>
#include <windows.h>
#include "native_path_internal.h"
#else
#include <unistd.h>
#endif

/* Provide the atomic process id operation used by this module and its client applications. */
static unsigned long umi_atomic_process_id(void)
{
#ifdef _WIN32
    return (unsigned long)_getpid();
#else
    return (unsigned long)getpid();
#endif
}

/* Provide the atomic flush operation used by this module and its client applications. */
static UmiStatus umi_atomic_flush(FILE *file)
{
    /* Apply this branch only when its contract condition is satisfied. */
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

/* Provide the atomic replace operation used by this module and its client applications. */
/* The UTF-8 boundary now uses wide Windows replacement and exclusive temporary ownership. The former implementation is retained for review. */
#if 0
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

#else
static UmiStatus umi_atomic_replace(const char *temporary, const char *destination)
{
#ifdef _WIN32
    wchar_t *source = NULL, *target = NULL;
    UmiStatus status = UmiNativePathWide(temporary, &source);
    if (status == UMI_STATUS_OK) status = UmiNativePathWide(destination, &target);
    if (status == UMI_STATUS_OK && !MoveFileExW(source, target,
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) status = UMI_STATUS_IO_ERROR;
    free(source);
    free(target);
    return status;
#else
    return rename(temporary, destination) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
#endif
}

/* Only call this for a temporary leaf successfully created by this operation.
 * Failed exclusive creation never grants ownership of somebody else's file. */
static void umi_atomic_remove_owned(const char *temporary)
{
#ifdef _WIN32
    wchar_t *native = NULL;
    if (UmiNativePathWide(temporary, &native) == UMI_STATUS_OK) {
        (void)_wremove(native);
        free(native);
    }
#else
    (void)remove(temporary);
#endif
}

static UmiStatus umi_atomic_open_exclusive(const char *path, FILE **out)
{
    *out = NULL;
#ifdef _WIN32
    wchar_t *native = NULL;
    UmiStatus status = UmiNativePathWide(path, &native);
    if (status != UMI_STATUS_OK) return status;
    int descriptor = _wopen(native, _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY | _O_NOINHERIT,
        _S_IREAD | _S_IWRITE);
    int error = errno;
    free(native);
#else
    int descriptor = open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
    int error = errno;
#endif
    if (descriptor < 0) {
        if (error == EEXIST) return UMI_STATUS_ALREADY_EXISTS;
        if (error == EACCES || error == EPERM) return UMI_STATUS_PERMISSION_DENIED;
        return UMI_STATUS_IO_ERROR;
    }
#ifdef _WIN32
    FILE *file = _wfdopen(descriptor, L"wb");
#else
    FILE *file = fdopen(descriptor, "wb");
#endif
    if (file == NULL) {
#ifdef _WIN32
        (void)_close(descriptor);
#else
        (void)close(descriptor);
#endif
        umi_atomic_remove_owned(path);
        return UMI_STATUS_IO_ERROR;
    }
    *out = file;
    return UMI_STATUS_OK;
}

/* Worker threads reserve distinct names without a data race. Refuse exhausted
 * tokens instead of wrapping to an earlier name. Exclusive creation separately
 * protects against earlier process runs and other filesystem writers. */
static UmiStatus umi_atomic_next_sequence(unsigned long *out)
{
    static atomic_ulong sequence = 0UL;
    unsigned long previous = atomic_load_explicit(&sequence, memory_order_relaxed);
    for (;;) {
        if (previous == ULONG_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (atomic_compare_exchange_weak_explicit(&sequence, &previous, previous + 1UL,
            memory_order_relaxed, memory_order_relaxed)) {
            *out = previous + 1UL;
            return UMI_STATUS_OK;
        }
    }
}
#endif
/*
 * Write atomic file in its stable representation and report capacity or input failures to
 * the caller.
 */
/* Exclusive UTF-8 temporary creation and checked sequence ownership replace truncating opens; flushing and atomic replacement remain required. The former implementation is retained for review. */
#if 0
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0' ||
        (data == NULL && size > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_fs_parent(parent, sizeof(parent), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (parent[0] != '\0') {
        status = umi_fs_make_directories(parent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(temporary)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    file = fopen(temporary, "wb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (size > 0U && fwrite(data, 1U, size, file) != size) {
        (void)fclose(file);
        (void)remove(temporary);
        return UMI_STATUS_IO_ERROR;
    }

    status = umi_atomic_flush(file);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (fclose(file) != 0 && status == UMI_STATUS_OK) {
        status = UMI_STATUS_IO_ERROR;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)remove(temporary);
        return status;
    }

    status = umi_atomic_replace(temporary, path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)remove(temporary);
    }
    return status;
}

#else
UmiStatus umi_atomic_file_write(const char *path, const void *data, size_t size)
{
    if (path == NULL || path[0] == '\0' || (data == NULL && size > 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    char current[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
    char temporary[UMI_PATH_CAPACITY], parent[UMI_PATH_CAPACITY];
    UmiStatus status = UMI_STATUS_OK;
    /* Resolve once: a later process-wide directory change cannot move one
     * phase of a save to a different directory. Parent links are still trusted. */
    if (umi_path_is_absolute(path)) status = umi_path_copy(destination, sizeof(destination), path);
    else {
        status = umi_fs_current_directory(current, sizeof(current));
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(path, current, destination, sizeof(destination));
    }
    if (status == UMI_STATUS_OK) status = umi_fs_parent(parent, sizeof(parent), destination);
    if (status == UMI_STATUS_OK && parent[0] != '\0') status = umi_fs_make_directories(parent);
    if (status != UMI_STATUS_OK) return status;
    FILE *file = NULL;
    for (unsigned attempt = 0U; attempt < 64U; ++attempt) {
        unsigned long sequence = 0UL;
        status = umi_atomic_next_sequence(&sequence);
        if (status != UMI_STATUS_OK) return status;
        int written = snprintf(temporary, sizeof(temporary), "%s.umi-tmp-%lu-%lu",
            destination, umi_atomic_process_id(), sequence);
        if (written < 0 || (size_t)written >= sizeof(temporary)) return UMI_STATUS_CAPACITY_EXCEEDED;
        status = umi_atomic_open_exclusive(temporary, &file);
        if (status != UMI_STATUS_ALREADY_EXISTS) break;
    }
    if (status != UMI_STATUS_OK) return status;
    if (size > 0U && fwrite(data, 1U, size, file) != size) status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) status = umi_atomic_flush(file);
    if (fclose(file) != 0 && status == UMI_STATUS_OK) status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) status = umi_atomic_replace(temporary, destination);
    if (status != UMI_STATUS_OK) umi_atomic_remove_owned(temporary);
    return status;
}
#endif
/*
 * Provide the atomic file write text operation used by this module and its client
 * applications.
 */
UmiStatus umi_atomic_file_write_text(const char *path,
                                     const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_atomic_file_write(path, text, strlen(text));
}
