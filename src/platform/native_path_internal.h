/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/native_path_internal.h
 * PURPOSE: Share explicit UTF-8 and UTF-16 conversion at native Windows filesystem boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_NATIVE_PATH_INTERNAL_H
#define UMICOM_PLATFORM_NATIVE_PATH_INTERNAL_H
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "umicom/base/status.h"

/* Application strings remain UTF-8 regardless of the user's Windows code page.
 * The caller owns a successful result and releases it with free. Reject malformed
 * text instead of silently opening a different name through replacement glyphs. */
static inline UmiStatus UmiNativePathWide(const char *text, wchar_t **out)
{
    if (text == NULL || text[0] == '\0' || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, NULL, 0);
    if (count == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > 32768)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    wchar_t *wide = malloc((size_t)count * sizeof(*wide));
    if (wide == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, wide, count) != count)
    {
        free(wide);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out = wide;
    return UMI_STATUS_OK;
}

/* Measure first, then publish a complete conversion. A failed lookup must not
 * replace an earlier valid path with a truncated or partially converted name. */
static inline UmiStatus UmiNativePathUtf8(const wchar_t *wide, char *out, size_t capacity)
{
    if (wide == NULL || out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    int count = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, NULL, 0, NULL, NULL);
    if (count == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if ((size_t)count > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *text = malloc((size_t)count);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide, -1, text, count, NULL, NULL) != count)
    {
        free(text);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memcpy(out, text, (size_t)count);
    free(text);
    return UMI_STATUS_OK;
}
#endif
#endif
