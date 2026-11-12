/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/input_stream.c
 *
 * PURPOSE:
 *   Keep streaming reads and native file identity checks in the shared platform layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#if !defined(_WIN32) && !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64
#endif
#include "umicom/platform/input_stream.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

UmiStatus UmiInputFileVisit(const char *path, uint64_t maximumBytes,
    UmiInputChunkVisitor visitor, void *context, uint64_t *outSize)
{
    if (outSize != NULL) *outSize = 0U;
    if (visitor == NULL || outSize == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    unsigned char buffer[65536];
    uint64_t size = 0U, received = 0U;
    status = UMI_STATUS_IO_ERROR;
#ifdef _WIN32
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (count <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    wchar_t *wide = calloc((size_t)count, sizeof(*wide));
    if (wide == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, count) != count) {
        free(wide);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    HANDLE file = CreateFileW(wide, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    free(wide);
    if (file == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND
            ? UMI_STATUS_NOT_FOUND : UMI_STATUS_IO_ERROR;
    }
    BY_HANDLE_FILE_INFORMATION before, after;
    LARGE_INTEGER length;
    if (GetFileType(file) != FILE_TYPE_DISK || !GetFileInformationByHandle(file, &before) ||
        (before.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0U ||
        !GetFileSizeEx(file, &length)) goto done;
    if (length.QuadPart < 0 || (uint64_t)length.QuadPart > maximumBytes) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    size = (uint64_t)length.QuadPart;
    while (received < size) {
        uint64_t remaining = size - received;
        DWORD amount = (DWORD)(remaining > sizeof(buffer) ? sizeof(buffer) : remaining);
        DWORD countRead = 0U;
        if (!ReadFile(file, buffer, amount, &countRead, NULL) || countRead == 0U) goto done;
        status = visitor(buffer, (size_t)countRead, context);
        if (status != UMI_STATUS_OK) goto done;
        received += countRead;
        status = UMI_STATUS_IO_ERROR;
    }
    if (!GetFileInformationByHandle(file, &after)) goto done;
    if (before.nFileSizeHigh != after.nFileSizeHigh || before.nFileSizeLow != after.nFileSizeLow ||
        CompareFileTime(&before.ftLastWriteTime, &after.ftLastWriteTime) != 0) {
        status = UMI_STATUS_BUSY;
        goto done;
    }
    status = UMI_STATUS_OK;
done:
    if (!CloseHandle(file)) status = UMI_STATUS_IO_ERROR;
#else
    /* Match the existing bounded reader's leaf policy. Streaming is additive:
     * UmiInputFileRead remains available for small owned buffers. */
    int file = open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK);
    if (file < 0) return errno == ENOENT ? UMI_STATUS_NOT_FOUND : UMI_STATUS_IO_ERROR;
    struct stat before, after;
    if (fstat(file, &before) != 0 || !S_ISREG(before.st_mode)) goto done;
    if (before.st_size < 0 || (uintmax_t)before.st_size > maximumBytes) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    size = (uint64_t)before.st_size;
    while (received < size) {
        uint64_t remaining = size - received;
        size_t amount = remaining > sizeof(buffer) ? sizeof(buffer) : (size_t)remaining;
        ssize_t countRead = read(file, buffer, amount);
        if (countRead < 0 && errno == EINTR) continue;
        if (countRead <= 0) goto done;
        status = visitor(buffer, (size_t)countRead, context);
        if (status != UMI_STATUS_OK) goto done;
        received += (uint64_t)countRead;
        status = UMI_STATUS_IO_ERROR;
    }
    if (fstat(file, &after) != 0) goto done;
#if defined(__APPLE__)
    if (before.st_size != after.st_size || before.st_mtimespec.tv_sec != after.st_mtimespec.tv_sec ||
        before.st_mtimespec.tv_nsec != after.st_mtimespec.tv_nsec ||
        before.st_ctimespec.tv_sec != after.st_ctimespec.tv_sec ||
        before.st_ctimespec.tv_nsec != after.st_ctimespec.tv_nsec) {
#else
    if (before.st_size != after.st_size || before.st_mtim.tv_sec != after.st_mtim.tv_sec ||
        before.st_mtim.tv_nsec != after.st_mtim.tv_nsec || before.st_ctim.tv_sec != after.st_ctim.tv_sec ||
        before.st_ctim.tv_nsec != after.st_ctim.tv_nsec) {
#endif
        status = UMI_STATUS_BUSY;
        goto done;
    }
    status = UMI_STATUS_OK;
done:
    if (close(file) != 0) status = UMI_STATUS_IO_ERROR;
#endif
    if (status == UMI_STATUS_OK) *outSize = size;
    return status;
}
