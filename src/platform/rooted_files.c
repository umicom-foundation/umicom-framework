/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/rooted_files.c
 *
 * PURPOSE:
 *   Keep workspace path validation, parent ownership and complete file publication in Framework.
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
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/output_file.h"
#include "workspace_name_internal.h"
#include <errno.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include "native_path_internal.h"
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

/* Only names, not process-global state, are shared between calls. Exclusive
 * staging creation handles counter wrap, previous crashes and other processes. */
/* Static scalar initialization is supported by C23. ATOMIC_VAR_INIT was
 * removed from that language mode; no runtime initialization is required. */
static atomic_ulong rootedSequence = 0UL;

typedef struct RootedParent
{
#ifdef _WIN32
    wchar_t path[UMI_PATH_CAPACITY];
    HANDLE parents[UMI_PATH_CAPACITY / 2U];
    size_t held;
#else
    int parent;
    char parts[UMI_PATH_CAPACITY];
    char *leaf;
#endif
} RootedParent;

#ifdef _WIN32
#include "rooted_files_win32.inc"
#include "rooted_create_win32.inc"
#include "rooted_directory_win32.inc"
#else
#include "rooted_files_posix.inc"
#include "rooted_create_posix.inc"
#include "rooted_directory_posix.inc"
#endif

/* Roots are existing absolute directories, including drive/share roots.
 * Validate without resolving '.', '..' or drive-relative spellings from hidden
 * process state. Reject malformed Windows UTF-8 before accessing a directory. */
static UmiStatus RootedValidateRoot(const char *root)
{
    if (root == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PATH_CAPACITY && root[length] != '\0')
        ++length;
    if (length == UMI_PATH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!umi_path_is_absolute(root))
        return UMI_STATUS_INVALID_ARGUMENT;
#ifndef _WIN32
    /* The shared path normaliser treats backslashes as separators. Refuse a
     * native POSIX name with that spelling rather than open a different root. */
    if (strchr(root, '\\') != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    size_t start = 1U, components = 0U;
    int network = 0;
#ifdef _WIN32
    wchar_t *native = NULL;
    UmiStatus status = UmiNativePathWide(root, &native);
    free(native);
    if (status != UMI_STATUS_OK)
        return status;
    network = (root[0] == '\\' || root[0] == '/') && (root[1] == '\\' || root[1] == '/');
    start = network ? 2U : 3U;
#endif
    for (size_t i = start; i <= length; ++i)
    {
        unsigned char value = (unsigned char)root[i];
        int separator = value == '/' || value == '\0';
#ifdef _WIN32
        separator = separator || value == '\\';
        if (value != 0U && (value < 32U || value == 127U || strchr(":*?\"<>|", value) != NULL))
            return UMI_STATUS_INVALID_ARGUMENT;
#endif
        if (!separator)
            continue;
        size_t count = i - start;
        if (count == 0U)
        {
            if (i == length && (components != 0U || !network))
                break;
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        if ((count == 1U && root[start] == '.') ||
            (count == 2U && root[start] == '.' && root[start + 1U] == '.'))
            return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
        if (root[i - 1U] == '.' || root[i - 1U] == ' ' || ReservedName(root + start, count))
            return UMI_STATUS_INVALID_ARGUMENT;
#endif
        ++components;
        start = i + 1U;
    }
    return network && components < 2U ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}

/* Web, asset and patch clients use the same lexical policy before deciding
 * whether to start I/O. Native operations repeat these checks at their boundary. */
UmiStatus UmiRootedFileValidatePath(const char *root, const char *relativePath)
{
    char normalized[UMI_PATH_CAPACITY];
    UmiStatus status = RootedValidateRoot(root);
    return status == UMI_STATUS_OK ? ValidateRelative(relativePath, normalized) : status;
}

/* Build all lexical state before opening a directory. A failed request never
 * creates a parent or tries an alternate spelling of an invalid filename. */
static UmiStatus RootedOpen(const char *root, const char *relative, RootedParent **out)
{
    *out = NULL;
    UmiStatus status = RootedValidateRoot(root);
    char normalised[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = ValidateRelative(relative, normalised);
    if (status != UMI_STATUS_OK)
        return status;
    RootedParent *parent = calloc(1U, sizeof(*parent));
    if (parent == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
#ifndef _WIN32
    parent->parent = -1;
#endif
    /* A trailing slash can make POSIX follow a root symlink before O_NOFOLLOW
     * sees it. Strip only redundant spelling after the root has been validated. */
    char normalRoot[UMI_PATH_CAPACITY];
    status = umi_path_normalise(root, normalRoot, sizeof(normalRoot));
    if (status == UMI_STATUS_OK)
        status = RootedOpenParent(normalRoot, normalised, parent);
    if (status != UMI_STATUS_OK)
    {
        RootedClose(parent);
        free(parent);
        return status;
    }
    *out = parent;
    return UMI_STATUS_OK;
}
static void RootedDispose(RootedParent *parent)
{
    if (parent != NULL)
    {
        RootedClose(parent);
        free(parent);
    }
}
void UmiRootedFileFree(void *bytes) { free(bytes); }

UmiStatus UmiRootedFileRead(const char *root, const char *relativePath, size_t maximumBytes,
                            unsigned char **outBytes, size_t *outSize)
{
    if (outBytes != NULL)
        *outBytes = NULL;
    if (outSize != NULL)
        *outSize = 0U;
    if (outBytes == NULL || outSize == NULL || maximumBytes == SIZE_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    RootedParent *parent = NULL;
    UmiStatus status = RootedOpen(root, relativePath, &parent);
    if (status == UMI_STATUS_OK)
        status = RootedRead(parent, maximumBytes, outBytes, outSize);
    RootedDispose(parent);
    return status;
}
UmiStatus UmiRootedFileWrite(const char *root, const char *relativePath, const void *bytes, size_t size)
{
    if (bytes == NULL && size != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    RootedParent *parent = NULL;
    UmiStatus status = RootedOpen(root, relativePath, &parent);
    if (status == UMI_STATUS_OK)
        status = RootedWrite(parent, bytes, size);
    RootedDispose(parent);
    return status;
}
UmiStatus UmiRootedFileRemove(const char *root, const char *relativePath)
{
    RootedParent *parent = NULL;
    UmiStatus status = RootedOpen(root, relativePath, &parent);
    if (status == UMI_STATUS_OK)
        status = RootedRemove(parent);
    RootedDispose(parent);
    return status;
}
UmiStatus UmiRootedFileExists(const char *root, const char *relativePath, int *outExists)
{
    if (outExists == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outExists = 0;
    RootedParent *parent = NULL;
    UmiStatus status = RootedOpen(root, relativePath, &parent);
    if (status == UMI_STATUS_OK)
        status = RootedExists(parent);
    RootedDispose(parent);
    if (status == UMI_STATUS_NOT_FOUND)
        return UMI_STATUS_OK;
    if (status == UMI_STATUS_OK)
        *outExists = 1;
    return status;
}

/* New-file export shares the rooted parent guards, but deliberately does not
 * call the replacement writer: a learning example must not replace user work. */
UmiStatus UmiRootedFileCreate(const char *root, const char *relativePath,
    const void *bytes, size_t size, UmiRootedFileCreation *report)
{
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *report = (UmiRootedFileCreation){0};
    report->status = UMI_STATUS_INVALID_ARGUMENT;
    if (bytes == NULL && size != 0U) return report->status;
    RootedParent *parent = NULL;
    UmiStatus status = RootedOpen(root, relativePath, &parent);
    if (status == UMI_STATUS_OK) status = RootedCreateNew(parent, bytes, size, report);
    RootedDispose(parent);
    report->status = status;
    return status;
}


/* Project recipes can be reviewed before a root exists. Share the exact name
 * admission used by native rooted operations without manufacturing a dummy
 * root or performing I/O. */
UmiStatus UmiRootedFileValidateRelativePath(const char *relativePath)
{
    char normalized[UMI_PATH_CAPACITY];
    return ValidateRelative(relativePath, normalized);
}

static unsigned char RootedPortableFold(unsigned char value)
{
    return value >= 'A' && value <= 'Z' ? (unsigned char)(value + ('a' - 'A')) : value;
}

UmiStatus UmiRootedFileCompareRelativePaths(const char *first, const char *second,
    UmiRootedPathRelation *outRelation)
{
    if (outRelation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char left[UMI_PATH_CAPACITY], right[UMI_PATH_CAPACITY];
    UmiStatus status = ValidateRelative(first, left);
    if (status == UMI_STATUS_OK) status = ValidateRelative(second, right);
    if (status != UMI_STATUS_OK) return status;
    size_t index = 0U;
    while (left[index] != '\0' && right[index] != '\0' &&
        RootedPortableFold((unsigned char)left[index]) ==
        RootedPortableFold((unsigned char)right[index])) ++index;
    UmiRootedPathRelation relation = UMI_ROOTED_PATH_DISJOINT;
    if (left[index] == '\0' && right[index] == '\0')
        relation = UMI_ROOTED_PATH_EQUAL;
    else if (left[index] == '\0' && right[index] == '/')
        relation = UMI_ROOTED_PATH_FIRST_PARENT;
    else if (right[index] == '\0' && left[index] == '/')
        relation = UMI_ROOTED_PATH_SECOND_PARENT;
    *outRelation = relation;
    return UMI_STATUS_OK;
}


/* Retain the original Ensure wrapper. Both public operations now use one
 * admission/allocation/report boundary with an explicit final-leaf policy. */
#if 0
/* Creation reuses the rooted file name/parent policy. It is a separate effect
 * from file publication so clients can report partial directory progress and
 * decide what to do next without hidden cleanup or recursive deletion. */
UmiStatus UmiRootedDirectoryEnsure(const char *root, const char *relativePath,
    UmiRootedDirectoryCreation *report)
{
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *report = (UmiRootedDirectoryCreation){0};
    report->status = UMI_STATUS_INVALID_ARGUMENT;
    char normalized[UMI_PATH_CAPACITY], normalRoot[UMI_PATH_CAPACITY];
    char joined[UMI_PATH_CAPACITY];
    UmiStatus status = RootedValidateRoot(root);
    if (status == UMI_STATUS_OK) status = ValidateRelative(relativePath, normalized);
    if (status == UMI_STATUS_OK) status = umi_path_normalise(root, normalRoot, sizeof normalRoot);
    if (status == UMI_STATUS_OK) status = umi_path_join(normalRoot, normalized, joined, sizeof joined);
    RootedParent *parent = NULL;
    if (status == UMI_STATUS_OK) {
        parent = calloc(1U, sizeof *parent);
        if (parent == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (parent != NULL) {
#ifndef _WIN32
        parent->parent = -1;
#endif
        status = RootedEnsureDirectories(normalRoot, normalized, parent, report);
        status = RootedDirectoryClose(parent, status);
        free(parent);
    }
    report->status = status;
    report->complete = status == UMI_STATUS_OK;
    return status;
}
#endif

/* Creation reuses the rooted file name/parent policy. It is a separate effect
 * from file publication so clients can report partial directory progress and
 * decide what to do next without hidden cleanup or recursive deletion. */
static UmiStatus RootedDirectoryPrepare(const char *root, const char *relativePath,
    UmiRootedDirectoryCreation *report, int exclusiveLeaf)
{
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *report = (UmiRootedDirectoryCreation){0};
    report->status = UMI_STATUS_INVALID_ARGUMENT;
    char normalized[UMI_PATH_CAPACITY], normalRoot[UMI_PATH_CAPACITY];
    char joined[UMI_PATH_CAPACITY];
    UmiStatus status = RootedValidateRoot(root);
    if (status == UMI_STATUS_OK) status = ValidateRelative(relativePath, normalized);
    if (status == UMI_STATUS_OK) status = umi_path_normalise(root, normalRoot, sizeof normalRoot);
    if (status == UMI_STATUS_OK) status = umi_path_join(normalRoot, normalized, joined, sizeof joined);
    RootedParent *parent = NULL;
    if (status == UMI_STATUS_OK) {
        parent = calloc(1U, sizeof *parent);
        if (parent == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (parent != NULL) {
#ifndef _WIN32
        parent->parent = -1;
#endif
        status = RootedPrepareDirectories(normalRoot, normalized, parent, report, exclusiveLeaf);
        status = RootedDirectoryClose(parent, status);
        free(parent);
    }
    report->status = status;
    report->complete = status == UMI_STATUS_OK;
    return status;
}

UmiStatus UmiRootedDirectoryEnsure(const char *root, const char *relativePath,
    UmiRootedDirectoryCreation *report)
{
    return RootedDirectoryPrepare(root, relativePath, report, 0);
}
UmiStatus UmiRootedDirectoryCreate(const char *root, const char *relativePath,
    UmiRootedDirectoryCreation *report)
{
    return RootedDirectoryPrepare(root, relativePath, report, 1);
}
