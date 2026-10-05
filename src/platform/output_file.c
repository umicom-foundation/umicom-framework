/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/output_file.c
 * PURPOSE: Own an exclusive native file handle for incremental raw output capture.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

struct UmiOutputFile {
    UmiOutputFileSnapshot snapshot;
#ifdef _WIN32
    HANDLE handle;
#else
    int descriptor;
#endif
};

#ifdef _WIN32
/* Win32 device aliases remain special even with an extension. Reject them
 * before native normalization, including superscript COM/LPT suffixes. */
static bool ordinary_component(const char *text, size_t length)
{
    if (length == 0U || text[length - 1U] == '.' || text[length - 1U] == ' ') return false;
    size_t stem = 0U;
    while (stem < length && text[stem] != '.') ++stem;
    while (stem != 0U && text[stem - 1U] == ' ') --stem;
    if (stem == 0U) return false;
    if (stem > 7U) return true;
    char name[8] = {0};
    for (size_t index = 0U; index < stem; ++index) {
        unsigned char value = (unsigned char)text[index];
        name[index] = (char)(value >= 'a' && value <= 'z' ? value - ('a' - 'A') : value);
    }
    if (strcmp(name, "CON") == 0 || strcmp(name, "NUL") == 0 || strcmp(name, "PRN") == 0 ||
        strcmp(name, "AUX") == 0 || strcmp(name, "CONIN$") == 0 || strcmp(name, "CONOUT$") == 0) return false;
    if (memcmp(name, "COM", 3U) == 0 || memcmp(name, "LPT", 3U) == 0) {
        if (stem == 4U && name[3] >= '0' && name[3] <= '9') return false;
        if (stem == 5U && (unsigned char)name[3] == 0xc2U &&
            ((unsigned char)name[4] == 0xb9U || (unsigned char)name[4] == 0xb2U || (unsigned char)name[4] == 0xb3U)) return false;
    }
    return true;
}

static UmiStatus native_error(DWORD error)
{
    if (error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS) return UMI_STATUS_ALREADY_EXISTS;
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return UMI_STATUS_NOT_FOUND;
    if (error == ERROR_ACCESS_DENIED || error == ERROR_SHARING_VIOLATION) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
#else
static UmiStatus native_error(int error)
{
    if (error == EEXIST) return UMI_STATUS_ALREADY_EXISTS;
    if (error == ENOENT) return UMI_STATUS_NOT_FOUND;
    if (error == EACCES || error == EPERM || error == ELOOP || error == ENOTDIR) return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_IO_ERROR;
}
#endif

/* Inspect bounded syntax without resolving against the process working folder. */
UmiStatus UmiOutputFileValidatePath(const char *path)
{
    if (path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PATH_CAPACITY && path[length] != '\0') ++length;
    if (length == UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t start;
#ifdef _WIN32
    if (length < 4U || !((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) ||
        path[1] != ':' || (path[2] != '/' && path[2] != '\\')) return UMI_STATUS_INVALID_ARGUMENT;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0) == 0) return UMI_STATUS_INVALID_ARGUMENT;
    start = 3U;
#else
    if (length < 2U || path[0] != '/') return UMI_STATUS_INVALID_ARGUMENT;
    start = 1U;
#endif
    for (size_t index = start; index <= length; ++index) {
        unsigned char value = (unsigned char)path[index];
        bool separator = value == '/' || value == '\0';
#ifdef _WIN32
        separator = separator || value == '\\';
        if (value != '\0' && (value < 32U || value == 127U || strchr(":*?\"<>|", value) != NULL)) return UMI_STATUS_INVALID_ARGUMENT;
#endif
        if (separator) {
            size_t count = index - start;
            if (count == 0U || (count == 1U && path[start] == '.') ||
                (count == 2U && path[start] == '.' && path[start + 1U] == '.')) return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
            if (!ordinary_component(path + start, count)) return UMI_STATUS_INVALID_ARGUMENT;
#endif
            start = index + 1U;
        }
    }
    return UMI_STATUS_OK;
}

/* Reserve the destination atomically. A prior existence check would race
 * another process and could overwrite the very file the user meant to keep. */
UmiStatus UmiOutputFileCreate(const char *path, UmiOutputFile **out_file)
{
    if (out_file == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_file = NULL;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    UmiOutputFile *file = calloc(1U, sizeof(*file));
    if (file == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    strcpy(file->snapshot.path, path);
#ifdef _WIN32
    wchar_t wide[UMI_PATH_CAPACITY];
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, (int)UMI_PATH_CAPACITY) == 0) {
        free(file); return UMI_STATUS_INVALID_ARGUMENT;
    }
    file->handle = CreateFileW(wide, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file->handle == INVALID_HANDLE_VALUE) { status = native_error(GetLastError()); free(file); return status; }
    if (GetFileType(file->handle) != FILE_TYPE_DISK) status = UMI_STATUS_INVALID_ARGUMENT;
#else
    file->descriptor = open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW, 0600);
    if (file->descriptor < 0) { status = native_error(errno); free(file); return status; }
    struct stat info;
    if (fstat(file->descriptor, &info) != 0 || !S_ISREG(info.st_mode)) status = UMI_STATUS_IO_ERROR;
#endif
    file->snapshot.status = status;
    if (status != UMI_STATUS_OK) { UmiOutputFileDestroy(file); return status; }
    *out_file = file;
    return UMI_STATUS_OK;
}

/* Native writes expose new bytes to readers without accumulating the whole
 * log in memory. Keep partial-write counts and stop after the first failure. */
UmiStatus UmiOutputFileWrite(UmiOutputFile *file, const void *bytes, size_t length)
{
    if (file == NULL || (bytes == NULL && length != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (file->snapshot.closed) return UMI_STATUS_INVALID_STATE;
    if (file->snapshot.status != UMI_STATUS_OK) return file->snapshot.status;
    if (length > UINT64_MAX - file->snapshot.bytes_written) {
        file->snapshot.status = UMI_STATUS_CAPACITY_EXCEEDED; return file->snapshot.status;
    }
    const unsigned char *cursor = bytes;
    while (length != 0U) {
        size_t chunk = length > 65536U ? 65536U : length;
        size_t written;
#ifdef _WIN32
        DWORD count = 0U;
        BOOL success = WriteFile(file->handle, cursor, (DWORD)chunk, &count, NULL);
        written = (size_t)count;
        if (!success) file->snapshot.status = UMI_STATUS_IO_ERROR;
#else
        ssize_t count;
        do { count = write(file->descriptor, cursor, chunk); } while (count < 0 && errno == EINTR);
        written = count > 0 ? (size_t)count : 0U;
        if (count < 0) file->snapshot.status = UMI_STATUS_IO_ERROR;
#endif
        file->snapshot.bytes_written += (uint64_t)written;
        if (written == 0U) file->snapshot.status = UMI_STATUS_IO_ERROR;
        if (file->snapshot.status != UMI_STATUS_OK) break;
        cursor += written; length -= written;
    }
    return file->snapshot.status;
}

/* Flush and close even after a write failure, keeping the first failure and
 * the useful prefix on disk. Do not retry close on a possibly reused handle. */
UmiStatus UmiOutputFileClose(UmiOutputFile *file)
{
    if (file == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (file->snapshot.closed) return file->snapshot.status;
    bool failed = false;
#ifdef _WIN32
    if (!FlushFileBuffers(file->handle)) failed = true;
    if (!CloseHandle(file->handle)) failed = true;
    file->handle = INVALID_HANDLE_VALUE;
#else
    int flushed;
    do { flushed = fsync(file->descriptor); } while (flushed != 0 && errno == EINTR);
    if (flushed != 0) failed = true;
    if (close(file->descriptor) != 0) failed = true;
    file->descriptor = -1;
#endif
    if (failed && file->snapshot.status == UMI_STATUS_OK) file->snapshot.status = UMI_STATUS_IO_ERROR;
    file->snapshot.closed = true;
    return file->snapshot.status;
}

/* A copied record lets callers publish progress using their own mutex. */
UmiStatus UmiOutputFileRead(const UmiOutputFile *file, UmiOutputFileSnapshot *out_snapshot)
{
    if (file == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_snapshot = file->snapshot; return UMI_STATUS_OK;
}

void UmiOutputFileDestroy(UmiOutputFile *file)
{
    if (file == NULL) return;
    (void)UmiOutputFileClose(file); free(file);
}
