/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/filesystem.c
 *
 * PURPOSE:
 *   Implement the filesystem behavior for
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
 * File: src/platform/filesystem.c
 *
 * PURPOSE:
 *   Implement the portable filesystem and path operations required by the
 *   Framework runtime, native Umicom command, repository scaffolding, build
 *   orchestration, and application products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/filesystem.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <direct.h>
#include <windows.h>
#define UMI_MKDIR(path) _mkdir(path)
#else
#include <limits.h>
#include <unistd.h>
#define UMI_MKDIR(path) mkdir((path), 0775)
#endif

static int umi_is_separator(char value)
{
    return value == '/' || value == '\\';
}

static UmiStatus umi_copy_string(char *destination,
                                 size_t capacity,
                                 const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

#ifdef _WIN32
static int umi_windows_delete_retryable(DWORD error)
{
    return error == ERROR_ACCESS_DENIED ||
           error == ERROR_SHARING_VIOLATION ||
           error == ERROR_LOCK_VIOLATION ||
           error == ERROR_DIR_NOT_EMPTY;
}

static void umi_windows_clear_read_only(const char *path)
{
    DWORD attributes = GetFileAttributesA(path);
    if (attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_READONLY) != 0U) {
        const DWORD writable_attributes =
            attributes & ~(DWORD)FILE_ATTRIBUTE_READONLY;
        (void)SetFileAttributesA(path, writable_attributes);
    }
}

static UmiStatus umi_windows_remove_file(const char *path)
{
    unsigned int attempt;
    umi_windows_clear_read_only(path);
    for (attempt = 0U; attempt < 40U; ++attempt) {
        DWORD error;
        if (DeleteFileA(path)) return UMI_STATUS_OK;
        error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
            return UMI_STATUS_OK;
        }
        if (!umi_windows_delete_retryable(error)) return UMI_STATUS_IO_ERROR;
        Sleep(25U);
    }
    return UMI_STATUS_IO_ERROR;
}

static UmiStatus umi_windows_remove_directory(const char *path)
{
    unsigned int attempt;
    umi_windows_clear_read_only(path);
    for (attempt = 0U; attempt < 40U; ++attempt) {
        DWORD error;
        if (RemoveDirectoryA(path)) return UMI_STATUS_OK;
        error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
            return UMI_STATUS_OK;
        }
        if (!umi_windows_delete_retryable(error)) return UMI_STATUS_IO_ERROR;
        Sleep(25U);
    }
    return UMI_STATUS_IO_ERROR;
}
#endif

UmiStatus umi_fs_read_bytes(const char *path,
                            unsigned char **out_bytes,
                            size_t *out_size)
{
    FILE *file;
    long length;
    unsigned char *bytes;
    size_t read_count;

    if (path == NULL || out_bytes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_bytes = NULL;
    if (out_size != NULL) {
        *out_size = 0U;
    }

    file = fopen(path, "rb");
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    length = ftell(file);
    if (length < 0L) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    rewind(file);
    bytes = (unsigned char *)malloc((size_t)length + 1U);
    if (bytes == NULL) {
        (void)fclose(file);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    read_count = fread(bytes, 1U, (size_t)length, file);
    (void)fclose(file);
    if (read_count != (size_t)length) {
        free(bytes);
        return UMI_STATUS_IO_ERROR;
    }
    bytes[read_count] = 0U;
    *out_bytes = bytes;
    if (out_size != NULL) {
        *out_size = read_count;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_fs_write_bytes(const char *path,
                             const void *bytes,
                             size_t size)
{
    FILE *file;
    if (path == NULL || (size > 0U && bytes == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    file = fopen(path, "wb");
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }
    if (size > 0U && fwrite(bytes, 1U, size, file) != size) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    return fclose(file) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

UmiStatus umi_fs_read_text(const char *path, char **out_text, size_t *out_size)
{
    unsigned char *bytes = NULL;
    UmiStatus status;
    if (out_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_fs_read_bytes(path, &bytes, out_size);
    if (status != UMI_STATUS_OK) {
        *out_text = NULL;
        return status;
    }
    *out_text = (char *)bytes;
    return UMI_STATUS_OK;
}

static UmiStatus umi_fs_write_mode(const char *path,
                                   const char *text,
                                   const char *mode)
{
    FILE *file;
    size_t length;

    if (path == NULL || text == NULL || mode == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, mode);
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    length = strlen(text);
    if (fwrite(text, 1U, length, file) != length) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }

    return fclose(file) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

UmiStatus umi_fs_write_text(const char *path, const char *text)
{
    return umi_fs_write_mode(path, text, "wb");
}

UmiStatus umi_fs_append_text(const char *path, const char *text)
{
    return umi_fs_write_mode(path, text, "ab");
}

int umi_fs_exists(const char *path)
{
    struct stat info;
    return path != NULL && stat(path, &info) == 0;
}

int umi_fs_is_file(const char *path)
{
    struct stat info;
    return path != NULL && stat(path, &info) == 0 && S_ISREG(info.st_mode);
}

int umi_fs_is_directory(const char *path)
{
    struct stat info;
    return path != NULL && stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

int umi_fs_is_absolute(const char *path)
{
    return umi_path_is_absolute(path);
}

UmiStatus umi_fs_normalise(char *path)
{
    char normalised[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_path_normalise(path, normalised, sizeof(normalised));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_copy_string(path, UMI_PATH_CAPACITY, normalised);
}

UmiStatus umi_fs_join(char *out_path,
                      size_t capacity,
                      const char *left,
                      const char *right)
{
    return umi_path_join(left, right, out_path, capacity);
}

UmiStatus umi_fs_parent(char *out_path,
                        size_t capacity,
                        const char *path)
{
    return umi_path_parent(path, out_path, capacity);
}

UmiStatus umi_fs_make_directories(const char *path)
{
    char working[UMI_PATH_CAPACITY];
    size_t index;
    size_t length;

    if (path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_copy_string(working, sizeof(working), path) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_fs_normalise(working);
    length = strlen(working);

    for (index = 1U; index < length; ++index) {
        if (umi_is_separator(working[index])) {
            char saved = working[index];
            working[index] = '\0';
#ifdef _WIN32
            if (!(index == 2U && working[1] == ':') &&
                working[0] != '\0' && !umi_fs_exists(working)) {
#else
            if (working[0] != '\0' && !umi_fs_exists(working)) {
#endif
                if (UMI_MKDIR(working) != 0 && errno != EEXIST) {
                    return UMI_STATUS_IO_ERROR;
                }
            }
            working[index] = saved;
        }
    }

    if (!umi_fs_exists(working) &&
        UMI_MKDIR(working) != 0 && errno != EEXIST) {
        return UMI_STATUS_IO_ERROR;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_fs_copy_file(const char *source, const char *destination)
{
    FILE *input;
    FILE *output;
    unsigned char buffer[16384];
    size_t count;
    char parent[UMI_PATH_CAPACITY];

    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_fs_parent(parent, sizeof(parent), destination) == UMI_STATUS_OK &&
        parent[0] != '\0') {
        (void)umi_fs_make_directories(parent);
    }

    input = fopen(source, "rb");
    if (input == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    output = fopen(destination, "wb");
    if (output == NULL) {
        (void)fclose(input);
        return UMI_STATUS_IO_ERROR;
    }

    while ((count = fread(buffer, 1U, sizeof(buffer), input)) > 0U) {
        if (fwrite(buffer, 1U, count, output) != count) {
            (void)fclose(input);
            (void)fclose(output);
            return UMI_STATUS_IO_ERROR;
        }
    }

    if (ferror(input) != 0) {
        (void)fclose(input);
        (void)fclose(output);
        return UMI_STATUS_IO_ERROR;
    }

    (void)fclose(input);
    return fclose(output) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

UmiStatus umi_fs_remove_tree(const char *path)
{
    DIR *directory;
    struct dirent *entry;

    if (path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!umi_fs_exists(path)) {
        return UMI_STATUS_OK;
    }

    if (umi_fs_is_file(path)) {
#ifdef _WIN32
        return umi_windows_remove_file(path);
#else
        return remove(path) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
#endif
    }

    directory = opendir(path);
    if (directory == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    while ((entry = readdir(directory)) != NULL) {
        char child[UMI_PATH_CAPACITY];
        UmiStatus status;

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        status = umi_fs_join(child, sizeof(child), path, entry->d_name);
        if (status != UMI_STATUS_OK) {
            (void)closedir(directory);
            return status;
        }

        status = umi_fs_remove_tree(child);
        if (status != UMI_STATUS_OK) {
            (void)closedir(directory);
            return status;
        }
    }

    (void)closedir(directory);
#ifdef _WIN32
    return umi_windows_remove_directory(path);
#else
    return rmdir(path) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
#endif
}

UmiStatus umi_fs_rename(const char *source, const char *destination)
{
    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return rename(source, destination) == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
}

UmiStatus umi_fs_current_directory(char *out_path, size_t capacity)
{
    if (out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#ifdef _WIN32
    return _getcwd(out_path, (int)capacity) != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
#else
    return getcwd(out_path, capacity) != NULL
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
#endif
}

UmiStatus umi_fs_temp_directory(char *out_path, size_t capacity)
{
    const char *value;

    if (out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#ifdef _WIN32
    value = getenv("TEMP");
    if (value == NULL || value[0] == '\0') {
        value = getenv("TMP");
    }
#else
    value = getenv("TMPDIR");
#endif
    if (value == NULL || value[0] == '\0') {
#ifdef _WIN32
        value = ".";
#else
        value = "/tmp";
#endif
    }
    return umi_copy_string(out_path, capacity, value);
}

UmiStatus umi_fs_executable_path(char *out_path, size_t capacity)
{
    if (out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#ifdef _WIN32
    {
        DWORD count = GetModuleFileNameA(NULL, out_path, (DWORD)capacity);
        if (count == 0U || count >= (DWORD)capacity) {
            return UMI_STATUS_IO_ERROR;
        }
        return UMI_STATUS_OK;
    }
#elif defined(__linux__)
    {
        ssize_t count = readlink("/proc/self/exe", out_path, capacity - 1U);
        if (count < 0 || (size_t)count >= capacity) {
            return UMI_STATUS_IO_ERROR;
        }
        out_path[(size_t)count] = '\0';
        return UMI_STATUS_OK;
    }
#else
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}

void umi_fs_free_bytes(void *bytes)
{
    free(bytes);
}

void umi_fs_free_text(char *text)
{
    free(text);
}
