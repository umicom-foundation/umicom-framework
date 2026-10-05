/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/local_files.c
 * PURPOSE: Share bounded text-file callbacks and captured workspace roots across coding integrations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_coding_runtime/local_workspace.h"
#include "umicom/ai_coding_runtime/path.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/rooted_files.h"
#include <string.h>

UmiStatus UmiAiCodingWorkspaceRootResolve(const char *root, char *outRoot, size_t capacity)
{
    if (root == NULL || root[0] == '\0' || outRoot == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
#ifndef _WIN32
    /* Backslash is a real POSIX filename character, while Framework paths use
     * it as a portable separator. Refuse an ambiguous root before normalization. */
    if (strchr(root, '\\') != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
#endif
    char absolute[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(root))
        status = umi_path_normalise(root, absolute, sizeof(absolute));
    else
    {
#ifdef _WIN32
        /* Drive-relative paths depend on hidden per-drive process state.
         * Require an explicit absolute drive or an ordinary relative path. */
        if (root[0] == '/' || root[0] == '\\' || strchr(root, ':') != NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
#endif
        char current[UMI_PATH_CAPACITY];
        status = umi_fs_current_directory(current, sizeof(current));
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(root, current, absolute, sizeof(absolute));
    }
    if (status != UMI_STATUS_OK)
        return status;
    size_t length = strlen(absolute);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(outRoot, absolute, length + 1U);
    return UMI_STATUS_OK;
}

/* Keep the coding runtime's relative-path syntax before applying the stricter
 * native regular-file policy. Existing patch approval remains a separate step. */
static UmiStatus LocalRelative(const char *relative, char *normalised)
{
    return umi_ai_coding_runtime_path_normalize_relative(relative, normalised,
                                                         UMI_AI_CODING_RUNTIME_PATH_CAPACITY);
}
UmiStatus UmiAiCodingWorkspaceReadFile(const char *root, const char *relativePath, char *outText,
                                       size_t capacity, size_t *outLength)
{
    if (outText != NULL && capacity != 0U)
        outText[0] = '\0';
    if (outLength != NULL)
        *outLength = 0U;
    if (outText == NULL || capacity == 0U || outLength == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    char relative[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    UmiStatus status = LocalRelative(relativePath, relative);
    unsigned char *bytes = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiRootedFileRead(root, relative, capacity - 1U, &bytes, &length);
    if (status == UMI_STATUS_OK && memchr(bytes, 0, length) != NULL)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
    {
        memcpy(outText, bytes, length + 1U);
        *outLength = length;
    }
    UmiRootedFileFree(bytes);
    return status;
}
UmiStatus UmiAiCodingWorkspaceWriteFile(const char *root, const char *relativePath, const char *text,
                                        size_t length)
{
    char relative[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    UmiStatus status = LocalRelative(relativePath, relative);
    return status == UMI_STATUS_OK ? UmiRootedFileWrite(root, relative, text, length) : status;
}
UmiStatus UmiAiCodingWorkspaceRemoveFile(const char *root, const char *relativePath)
{
    char relative[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    UmiStatus status = LocalRelative(relativePath, relative);
    return status == UMI_STATUS_OK ? UmiRootedFileRemove(root, relative) : status;
}
UmiStatus UmiAiCodingWorkspaceFileExists(const char *root, const char *relativePath, int *outExists)
{
    if (outExists == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outExists = 0;
    char relative[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    UmiStatus status = LocalRelative(relativePath, relative);
    return status == UMI_STATUS_OK ? UmiRootedFileExists(root, relative, outExists) : status;
}
