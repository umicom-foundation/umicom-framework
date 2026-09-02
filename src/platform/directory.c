/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/directory.c
 *
 * PURPOSE:
 *   Implement the directory behavior for
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
 * File: src/platform/directory.c
 *
 * PURPOSE:
 *   Implement sorted, bounded directory traversal and portable file metadata
 *   collection for Windows and POSIX systems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/directory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

#define UMI_DIRECTORY_NAME_CAPACITY 1024U

typedef struct UmiNameList {
    char **items;
    size_t count;
    size_t capacity;
} UmiNameList;

/* Provide the compare names operation used by this module and its client applications. */
static int compare_names(const void *left, const void *right)
{
    const char *const *a = (const char *const *)left;
    const char *const *b = (const char *const *)right;
#ifdef _WIN32
    return _stricmp(*a, *b);
#else
    return strcmp(*a, *b);
#endif
}

/* Release or reset state held by name list so the same storage can be reused safely. */
static void name_list_dispose(UmiNameList *list)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) {
        free(list->items[index]);
    }
    free(list->items);
    (void)memset(list, 0, sizeof(*list));
}

/* Add name list only after its inputs and available capacity have been checked. */
static UmiStatus name_list_add(UmiNameList *list, const char *name)
{
    char *copy;
    char **resized;
    size_t capacity;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->count == list->capacity) {
        capacity = list->capacity == 0U ? 32U : list->capacity * 2U;
        resized = (char **)realloc(list->items, capacity * sizeof(*resized));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        list->items = resized;
        list->capacity = capacity;
    }
    copy = (char *)malloc(strlen(name) + 1U);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)strcpy(copy, name);
    list->items[list->count++] = copy;
    return UMI_STATUS_OK;
}

/* Provide the is hidden name operation used by this module and its client applications. */
static int is_hidden_name(const char *name)
{
    return name != NULL && name[0] == '.' &&
           strcmp(name, ".") != 0 && strcmp(name, "..") != 0;
}

/*
 * Provide the directory walk options default operation used by this module and its client
 * applications.
 */
UmiDirectoryWalkOptions umi_directory_walk_options_default(void)
{
    UmiDirectoryWalkOptions options;
    options.max_depth = 64U;
    options.recursive = 1;
    options.include_files = 1;
    options.include_directories = 0;
    options.include_hidden = 0;
    options.follow_symbolic_links = 0;
    return options;
}

/* Provide the directory stat operation used by this module and its client applications. */
UmiStatus umi_directory_stat(const char *path, UmiFileInfo *out_info)
{
    char name[UMI_PATH_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_info == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_info, 0, sizeof(*out_info));
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_path_normalise(path,
                           out_info->path,
                           sizeof(out_info->path)) != UMI_STATUS_OK ||
        umi_path_basename(path, name, sizeof(name)) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(out_info->name, sizeof(out_info->name), "%s", name);
    out_info->hidden = is_hidden_name(name);
#ifdef _WIN32
    {
        WIN32_FILE_ATTRIBUTE_DATA data;
        ULARGE_INTEGER size;
        ULARGE_INTEGER time;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!GetFileAttributesExA(path, GetFileExInfoStandard, &data)) {
            return UMI_STATUS_NOT_FOUND;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0U) {
            out_info->kind = UMI_FILE_KIND_SYMBOLIC_LINK;
        } else /* Apply this branch only when its contract condition is satisfied. */ if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0U) {
            out_info->kind = UMI_FILE_KIND_DIRECTORY;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_info->kind = UMI_FILE_KIND_REGULAR;
        }
        size.HighPart = data.nFileSizeHigh;
        size.LowPart = data.nFileSizeLow;
        out_info->size = size.QuadPart;
        time.HighPart = data.ftLastWriteTime.dwHighDateTime;
        time.LowPart = data.ftLastWriteTime.dwLowDateTime;
        out_info->modified_nanoseconds = time.QuadPart * 100ULL;
        out_info->hidden = out_info->hidden ||
            (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0U;
    }
#else
    {
        struct stat info;
        /* Apply this branch only when its contract condition is satisfied. */
        if (lstat(path, &info) != 0) {
            return UMI_STATUS_NOT_FOUND;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (S_ISREG(info.st_mode)) out_info->kind = UMI_FILE_KIND_REGULAR;
        else /* Apply this branch only when its contract condition is satisfied. */ if (S_ISDIR(info.st_mode)) out_info->kind = UMI_FILE_KIND_DIRECTORY;
        else /* Apply this branch only when its contract condition is satisfied. */ if (S_ISLNK(info.st_mode)) out_info->kind = UMI_FILE_KIND_SYMBOLIC_LINK;
        /* Use this fallback path when the earlier condition does not apply. */
        else out_info->kind = UMI_FILE_KIND_OTHER;
        out_info->size = info.st_size > 0 ? (uint64_t)info.st_size : 0U;
#if defined(__APPLE__)
        out_info->modified_nanoseconds =
            (uint64_t)info.st_mtimespec.tv_sec * 1000000000ULL +
            (uint64_t)info.st_mtimespec.tv_nsec;
#else
        out_info->modified_nanoseconds =
            (uint64_t)info.st_mtim.tv_sec * 1000000000ULL +
            (uint64_t)info.st_mtim.tv_nsec;
#endif
    }
#endif
    return UMI_STATUS_OK;
}

/* Provide the collect names operation used by this module and its client applications. */
static UmiStatus collect_names(const char *directory, UmiNameList *out_list)
{
#ifdef _WIN32
    char pattern[UMI_PATH_CAPACITY];
    WIN32_FIND_DATAA data;
    HANDLE handle;
    UmiStatus status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_path_join(directory, "*", pattern, sizeof(pattern)) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    handle = FindFirstFileA(pattern, &data);
    /* Apply this operation only while the related capability or state is available. */
    if (handle == INVALID_HANDLE_VALUE) {
        return UMI_STATUS_IO_ERROR;
    }
    do {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(data.cFileName, ".") == 0 ||
            strcmp(data.cFileName, "..") == 0) {
            continue;
        }
        status = name_list_add(out_list, data.cFileName);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            FindClose(handle);
            return status;
        }
    } /* Continue only while work remains available; the loop body advances the state on each pass. */ while (FindNextFileA(handle, &data));
    FindClose(handle);
    return UMI_STATUS_OK;
#else
    DIR *stream;
    struct dirent *entry;
    UmiStatus status;
    stream = opendir(directory);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL) {
        return UMI_STATUS_IO_ERROR;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while ((entry = readdir(stream)) != NULL) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        status = name_list_add(out_list, entry->d_name);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)closedir(stream);
            return status;
        }
    }
    (void)closedir(stream);
    return UMI_STATUS_OK;
#endif
}

/* Provide the walk directory operation used by this module and its client applications. */
static UmiStatus walk_directory(const char *directory,
                                size_t depth,
                                const UmiDirectoryWalkOptions *options,
                                UmiDirectoryVisitor visitor,
                                void *user_data)
{
    UmiNameList names;
    size_t index;
    UmiStatus status;
    (void)memset(&names, 0, sizeof(names));
    status = collect_names(directory, &names);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        name_list_dispose(&names);
        return status;
    }
    qsort(names.items, names.count, sizeof(*names.items), compare_names);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < names.count; ++index) {
        char path[UMI_PATH_CAPACITY];
        UmiFileInfo info;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!options->include_hidden && is_hidden_name(names.items[index])) {
            continue;
        }
        status = umi_path_join(directory,
                               names.items[index],
                               path,
                               sizeof(path));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        status = umi_directory_stat(path, &info);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        info.depth = depth;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((info.kind == UMI_FILE_KIND_REGULAR && options->include_files) ||
            (info.kind == UMI_FILE_KIND_DIRECTORY &&
             options->include_directories) ||
            (info.kind == UMI_FILE_KIND_SYMBOLIC_LINK &&
             options->follow_symbolic_links && options->include_files)) {
            status = visitor(&info, user_data);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (options->recursive && depth < options->max_depth &&
            info.kind == UMI_FILE_KIND_DIRECTORY) {
            status = walk_directory(path,
                                    depth + 1U,
                                    options,
                                    visitor,
                                    user_data);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
        }
    }
    name_list_dispose(&names);
    return status;
}

/* Provide the directory walk operation used by this module and its client applications. */
UmiStatus umi_directory_walk(const char *root,
                             const UmiDirectoryWalkOptions *options,
                             UmiDirectoryVisitor visitor,
                             void *user_data)
{
    UmiDirectoryWalkOptions effective;
    UmiFileInfo root_info;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || visitor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = options != NULL
        ? *options
        : umi_directory_walk_options_default();
    status = umi_directory_stat(root, &root_info);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (root_info.kind != UMI_FILE_KIND_DIRECTORY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return walk_directory(root, 0U, &effective, visitor, user_data);
}

typedef struct UmiCountContext { size_t count; } UmiCountContext;

/* Provide the count visitor operation used by this module and its client applications. */
static UmiStatus count_visitor(const UmiFileInfo *info, void *user_data)
{
    UmiCountContext *context = (UmiCountContext *)user_data;
    (void)info;
    context->count += 1U;
    return UMI_STATUS_OK;
}

/* Return the number of records represented by directory without changing their state. */
UmiStatus umi_directory_count(const char *root,
                              const UmiDirectoryWalkOptions *options,
                              size_t *out_count)
{
    UmiCountContext context;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_count == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    context.count = 0U;
    status = umi_directory_walk(root, options, count_visitor, &context);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK) *out_count = context.count;
    return status;
}
