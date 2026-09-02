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

/* Provide the is separator operation used by this module and its client applications. */
static int umi_is_separator(char value)
{
    return value == '/' || value == '\\';
}

/* Provide the copy string operation used by this module and its client applications. */
static UmiStatus umi_copy_string(char *destination,
                                 size_t capacity,
                                 const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

#ifdef _WIN32
/*
 * Provide the windows delete retryable operation used by this module and its client
 * applications.
 */
static int umi_windows_delete_retryable(DWORD error)
{
    return error == ERROR_ACCESS_DENIED ||
           error == ERROR_SHARING_VIOLATION ||
           error == ERROR_LOCK_VIOLATION ||
           error == ERROR_DIR_NOT_EMPTY;
}

/*
 * Provide the windows clear read only operation used by this module and its client
 * applications.
 */
static void umi_windows_clear_read_only(const char *path)
{
    DWORD attributes = GetFileAttributesA(path);
    /* Apply this operation only while the related capability or state is available. */
    if (attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_READONLY) != 0U) {
        const DWORD writable_attributes =
            attributes & ~(DWORD)FILE_ATTRIBUTE_READONLY;
        (void)SetFileAttributesA(path, writable_attributes);
    }
}

/*
 * Provide the windows remove file operation used by this module and its client
 * applications.
 */
static UmiStatus umi_windows_remove_file(const char *path)
{
    unsigned int attempt;
    umi_windows_clear_read_only(path);
    /* Visit each bounded item once so every record receives the same rule. */
    for (attempt = 0U; attempt < 40U; ++attempt) {
        DWORD error;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (DeleteFileA(path)) return UMI_STATUS_OK;
        error = GetLastError();
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
            return UMI_STATUS_OK;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!umi_windows_delete_retryable(error)) return UMI_STATUS_IO_ERROR;
        Sleep(25U);
    }
    return UMI_STATUS_IO_ERROR;
}

/*
 * Provide the windows remove directory operation used by this module and its client
 * applications.
 */
static UmiStatus umi_windows_remove_directory(const char *path)
{
    unsigned int attempt;
    umi_windows_clear_read_only(path);
    /* Visit each bounded item once so every record receives the same rule. */
    for (attempt = 0U; attempt < 40U; ++attempt) {
        DWORD error;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (RemoveDirectoryA(path)) return UMI_STATUS_OK;
        error = GetLastError();
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
            return UMI_STATUS_OK;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!umi_windows_delete_retryable(error)) return UMI_STATUS_IO_ERROR;
        Sleep(25U);
    }
    return UMI_STATUS_IO_ERROR;
}
#endif

/* Provide the fs read bytes operation used by this module and its client applications. */
UmiStatus umi_fs_read_bytes(const char *path,
                            unsigned char **out_bytes,
                            size_t *out_size)
{
    FILE *file;
    long length;
    unsigned char *bytes;
    size_t read_count;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_bytes == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_bytes = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_size != NULL) {
        *out_size = 0U;
    }

    file = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (fseek(file, 0, SEEK_END) != 0) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    length = ftell(file);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0L) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    rewind(file);
    bytes = (unsigned char *)malloc((size_t)length + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL) {
        (void)fclose(file);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    read_count = fread(bytes, 1U, (size_t)length, file);
    (void)fclose(file);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (read_count != (size_t)length) {
        free(bytes);
        return UMI_STATUS_IO_ERROR;
    }
    bytes[read_count] = 0U;
    *out_bytes = bytes;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_size != NULL) {
        *out_size = read_count;
    }
    return UMI_STATUS_OK;
}

/* Provide the fs write bytes operation used by this module and its client applications. */
UmiStatus umi_fs_write_bytes(const char *path,
                             const void *bytes,
                             size_t size)
{
    FILE *file;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || (size > 0U && bytes == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    file = fopen(path, "wb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (size > 0U && fwrite(bytes, 1U, size, file) != size) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }
    return fclose(file) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/* Provide the fs read text operation used by this module and its client applications. */
UmiStatus umi_fs_read_text(const char *path, char **out_text, size_t *out_size)
{
    unsigned char *bytes = NULL;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_fs_read_bytes(path, &bytes, out_size);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        *out_text = NULL;
        return status;
    }
    *out_text = (char *)bytes;
    return UMI_STATUS_OK;
}

/* Provide the fs write mode operation used by this module and its client applications. */
static UmiStatus umi_fs_write_mode(const char *path,
                                   const char *text,
                                   const char *mode)
{
    FILE *file;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || text == NULL || mode == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = fopen(path, mode);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (file == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (fwrite(text, 1U, length, file) != length) {
        (void)fclose(file);
        return UMI_STATUS_IO_ERROR;
    }

    return fclose(file) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/* Provide the fs write text operation used by this module and its client applications. */
UmiStatus umi_fs_write_text(const char *path, const char *text)
{
    return umi_fs_write_mode(path, text, "wb");
}

/* Provide the fs append text operation used by this module and its client applications. */
UmiStatus umi_fs_append_text(const char *path, const char *text)
{
    return umi_fs_write_mode(path, text, "ab");
}

/* Provide the fs exists operation used by this module and its client applications. */
int umi_fs_exists(const char *path)
{
    struct stat info;
    return path != NULL && stat(path, &info) == 0;
}

/* Provide the fs is file operation used by this module and its client applications. */
int umi_fs_is_file(const char *path)
{
    struct stat info;
    return path != NULL && stat(path, &info) == 0 && S_ISREG(info.st_mode);
}

/* Provide the fs is directory operation used by this module and its client applications. */
int umi_fs_is_directory(const char *path)
{
    struct stat info;
    return path != NULL && stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

/* Provide the fs is absolute operation used by this module and its client applications. */
int umi_fs_is_absolute(const char *path)
{
    return umi_path_is_absolute(path);
}

/* Provide the fs normalise operation used by this module and its client applications. */
UmiStatus umi_fs_normalise(char *path)
{
    char normalised[UMI_PATH_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_path_normalise(path, normalised, sizeof(normalised));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_copy_string(path, UMI_PATH_CAPACITY, normalised);
}

/* Provide the fs join operation used by this module and its client applications. */
UmiStatus umi_fs_join(char *out_path,
                      size_t capacity,
                      const char *left,
                      const char *right)
{
    return umi_path_join(left, right, out_path, capacity);
}

/* Provide the fs parent operation used by this module and its client applications. */
UmiStatus umi_fs_parent(char *out_path,
                        size_t capacity,
                        const char *path)
{
    return umi_path_parent(path, out_path, capacity);
}

/*
 * Provide the fs make directories operation used by this module and its client
 * applications.
 */
UmiStatus umi_fs_make_directories(const char *path)
{
    char working[UMI_PATH_CAPACITY];
    size_t index;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_copy_string(working, sizeof(working), path) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_fs_normalise(working);
    length = strlen(working);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < length; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_is_separator(working[index])) {
            char saved = working[index];
            working[index] = '\0';
#ifdef _WIN32
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!(index == 2U && working[1] == ':') &&
                working[0] != '\0' && !umi_fs_exists(working)) {
#else
            /* Apply this branch only when its contract condition is satisfied. */
            if (working[0] != '\0' && !umi_fs_exists(working)) {
#endif
                /* Apply this branch only when its contract condition is satisfied. */
                if (UMI_MKDIR(working) != 0 && errno != EEXIST) {
                    return UMI_STATUS_IO_ERROR;
                }
            }
            working[index] = saved;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_fs_parent(parent, sizeof(parent), destination) == UMI_STATUS_OK &&
        parent[0] != '\0') {
        (void)umi_fs_make_directories(parent);
    }

    input = fopen(source, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (input == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    output = fopen(destination, "wb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL) {
        (void)fclose(input);
        return UMI_STATUS_IO_ERROR;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((count = fread(buffer, 1U, sizeof(buffer), input)) > 0U) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (fwrite(buffer, 1U, count, output) != count) {
            (void)fclose(input);
            (void)fclose(output);
            return UMI_STATUS_IO_ERROR;
        }
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_fs_exists(path)) {
        return UMI_STATUS_OK;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_fs_is_file(path)) {
#ifdef _WIN32
        return umi_windows_remove_file(path);
#else
        return remove(path) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
#endif
    }

    directory = opendir(path);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (directory == NULL) {
        return UMI_STATUS_IO_ERROR;
    }

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((entry = readdir(directory)) != NULL) {
        char child[UMI_PATH_CAPACITY];
        UmiStatus status;

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        status = umi_fs_join(child, sizeof(child), path, entry->d_name);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)closedir(directory);
            return status;
        }

        status = umi_fs_remove_tree(child);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return rename(source, destination) == 0
        ? UMI_STATUS_OK
        : UMI_STATUS_IO_ERROR;
}

UmiStatus umi_fs_current_directory(char *out_path, size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#ifdef _WIN32
    value = getenv("TEMP");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value[0] == '\0') {
        value = getenv("TMP");
    }
#else
    value = getenv("TMPDIR");
#endif
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#ifdef _WIN32
    {
        DWORD count = GetModuleFileNameA(NULL, out_path, (DWORD)capacity);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count == 0U || count >= (DWORD)capacity) {
            return UMI_STATUS_IO_ERROR;
        }
        return UMI_STATUS_OK;
    }
#elif defined(__linux__)
    {
        ssize_t count = readlink("/proc/self/exe", out_path, capacity - 1U);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
