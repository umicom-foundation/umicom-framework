/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/audio_file.c
 * PURPOSE: Read a bounded, explicitly selected local audio file without writes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#include "audio_internal.h"
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
UmiStatus UmiCreativeAudioLoadFile(const char *path, UmiCreativeAudioClip **out)
{
    if (out == NULL || !UmiCreativeLocalPathValid(path)) return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL) return UMI_STATUS_INVALID_STATE;
    unsigned char *bytes = NULL;
    size_t size = 0U, readBytes = 0U;
    UmiStatus status = UMI_STATUS_IO_ERROR;
#ifdef _WIN32
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (count <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    wchar_t *wide = calloc((size_t)count, sizeof(*wide));
    if (wide == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, count) != count) {
        free(wide); return UMI_STATUS_INVALID_ARGUMENT;
    }
    HANDLE file = CreateFileW(wide, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    free(wide);
    if (file == INVALID_HANDLE_VALUE) return UMI_STATUS_IO_ERROR;
    BY_HANDLE_FILE_INFORMATION info;
    LARGE_INTEGER length;
    if (GetFileType(file) != FILE_TYPE_DISK || !GetFileInformationByHandle(file, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0U ||
        !GetFileSizeEx(file, &length)) goto done;
    if (length.QuadPart < 0 || (unsigned long long)length.QuadPart > UMI_CREATIVE_EXPORT_LIMIT) {
        status = UMI_STATUS_CAPACITY_EXCEEDED; goto done;
    }
    size = (size_t)length.QuadPart;
    if (size == 0U) { status = UMI_STATUS_PARSE_ERROR; goto done; }
    bytes = malloc(size);
    if (bytes == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    while (readBytes < size) {
        DWORD received = 0U;
        if (!ReadFile(file, bytes + readBytes, (DWORD)(size - readBytes), &received, NULL) || received == 0U) goto done;
        readBytes += received;
    }
    status = UMI_STATUS_OK;
done:
    if (!CloseHandle(file)) status = UMI_STATUS_IO_ERROR;
#else
    /* Nonblocking open prevents a FIFO from blocking before fstat rejects it.
     * O_NOFOLLOW applies to the final path component, not its parent folders. */
    int file = open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK);
    if (file < 0) return UMI_STATUS_IO_ERROR;
    struct stat before, after;
    if (fstat(file, &before) != 0 || !S_ISREG(before.st_mode)) goto done;
    if (before.st_size < 0 || (uintmax_t)before.st_size > UMI_CREATIVE_EXPORT_LIMIT) {
        status = UMI_STATUS_CAPACITY_EXCEEDED; goto done;
    }
    size = (size_t)before.st_size;
    if (size == 0U) { status = UMI_STATUS_PARSE_ERROR; goto done; }
    bytes = malloc(size);
    if (bytes == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    while (readBytes < size) {
        ssize_t received = read(file, bytes + readBytes, size - readBytes);
        if (received < 0 && errno == EINTR) continue;
        if (received <= 0) goto done;
        readBytes += (size_t)received;
    }
    if (fstat(file, &after) != 0 || after.st_size != before.st_size ||
        after.st_mtim.tv_sec != before.st_mtim.tv_sec || after.st_mtim.tv_nsec != before.st_mtim.tv_nsec ||
        after.st_ctim.tv_sec != before.st_ctim.tv_sec || after.st_ctim.tv_nsec != before.st_ctim.tv_nsec) {
        status = UMI_STATUS_BUSY; goto done;
    }
    status = UMI_STATUS_OK;
done:
    if (close(file) != 0) status = UMI_STATUS_IO_ERROR;
#endif
    if (status == UMI_STATUS_OK) status = UmiCreativeAudioDecode(bytes, size, out);
    free(bytes);
    return status;
}
