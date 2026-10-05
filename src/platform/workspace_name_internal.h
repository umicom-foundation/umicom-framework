/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace_name_internal.h
 * PURPOSE: Share portable workspace entry validation between creation and bounded file access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_WORKSPACE_NAME_INTERNAL_H
#define UMICOM_PLATFORM_WORKSPACE_NAME_INTERNAL_H
#include <ctype.h>
#include <string.h>
#include "umicom/platform/path.h"
/* Windows device aliases are invalid even with an extension, so a project
 * created on Linux can be checked out and edited on Windows as well. */
/* Windows also reserves console handles and superscript serial-port aliases.
 * Apply the same portable-name rule on every host so moving a project cannot
 * turn an ordinary document into an operating-system device path. */
static inline int ReservedName(const char *name, size_t length)
{
    size_t count = 0U;
    while (count < length && name[count] != '.')
        ++count;
    while (count != 0U && name[count - 1U] == ' ')
        --count;
    if (count > 7U)
        return 0;
    char stem[8] = {0};
    for (size_t i = 0U; i < count; ++i)
    {
        unsigned char value = (unsigned char)name[i];
        stem[i] = (char)(value >= 'a' && value <= 'z' ? value - ('a' - 'A') : value);
    }
    if (strcmp(stem, "CON") == 0 || strcmp(stem, "PRN") == 0 || strcmp(stem, "AUX") == 0 ||
        strcmp(stem, "NUL") == 0 || strcmp(stem, "CONIN$") == 0 || strcmp(stem, "CONOUT$") == 0)
        return 1;
    if (memcmp(stem, "COM", 3U) != 0 && memcmp(stem, "LPT", 3U) != 0)
        return 0;
    return (count == 4U && stem[3] >= '0' && stem[3] <= '9') ||
           (count == 5U && (unsigned char)stem[3] == 0xc2U &&
            ((unsigned char)stem[4] == 0xb9U || (unsigned char)stem[4] == 0xb2U ||
             (unsigned char)stem[4] == 0xb3U));
}

/* Validate before normalising so parent traversal is never erased by cleanup.
 * This only checks path syntax; the OS operations also check actual parents. */
static inline UmiStatus ValidateRelative(const char *path, char *normalised)
{
    size_t length = 0U;
    size_t start = 0U;
    if (path == NULL || path[0] == '\0' || path[0] == '/' || path[0] == '\\')
        return UMI_STATUS_INVALID_ARGUMENT;
    while (length < UMI_PATH_CAPACITY && path[length] != '\0')
        ++length;
    if (length == UMI_PATH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (path[length - 1U] == '/' || path[length - 1U] == '\\')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t pos = 0U; pos <= length; ++pos)
    {
        unsigned char value = (unsigned char)path[pos];
        if (value == '\0' || value == '/' || value == '\\')
        {
            size_t count = pos - start;
            if (count == 0U || (count == 1U && path[start] == '.') ||
                (count == 2U && path[start] == '.' && path[start + 1U] == '.') || path[pos - 1U] == '.' ||
                path[pos - 1U] == ' ' || ReservedName(path + start, count))
                return UMI_STATUS_INVALID_ARGUMENT;
            if (count == 4U && path[start] == '.' && tolower((unsigned char)path[start + 1U]) == 'g' &&
                tolower((unsigned char)path[start + 2U]) == 'i' &&
                tolower((unsigned char)path[start + 3U]) == 't')
                return UMI_STATUS_PERMISSION_DENIED;
            start = pos + 1U;
        }
        else if (value < 32U || value == 127U || strchr("<>:\"|?*", value) != NULL)
        {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return umi_path_normalise(path, normalised, UMI_PATH_CAPACITY);
}

#endif
