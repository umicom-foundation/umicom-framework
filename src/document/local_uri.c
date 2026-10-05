/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/local_uri.c
 * PURPOSE: Decode local file targets into validated absolute UTF-8 paths before document navigation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/local_uri.h"
#include "umicom/document/text_encoding.h"
#include <string.h>
#ifdef _WIN32
#include "umicom/platform/output_file.h"
#endif
static unsigned char LocalUriLower(unsigned char value)
{
    return value >= 'A' && value <= 'Z' ? (unsigned char)(value + ('a' - 'A')) : value;
}
static int LocalUriEqual(const char *text, size_t length, const char *expected)
{
    if (strlen(expected) != length)
        return 0;
    for (size_t i = 0U; i < length; ++i)
        if (LocalUriLower((unsigned char)text[i]) != (unsigned char)expected[i])
            return 0;
    return 1;
}
static int LocalUriHex(unsigned char value)
{
    if (value >= '0' && value <= '9')
        return (int)(value - '0');
    value = LocalUriLower(value);
    return value >= 'a' && value <= 'f' ? (int)(value - 'a') + 10 : -1;
}
/* Local Windows source targets share platform filename validation before any provider can open them.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentLocalFileUriToPath(const char *uri, char *out_path, size_t capacity)
{
    if (uri == NULL || out_path == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = 0U;
    while (bytes <= 8192U && uri[bytes] != '\0')
        ++bytes;
    if (bytes > 8192U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *colon = memchr(uri, ':', bytes);
    if (colon == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!LocalUriEqual(uri, (size_t)(colon - uri), "file"))
        return UMI_STATUS_NOT_IMPLEMENTED;
    size_t begin = (size_t)(colon - uri) + 1U;
    if (bytes - begin < 3U || uri[begin] != '/' || uri[begin + 1U] != '/')
        return UMI_STATUS_INVALID_ARGUMENT;
    begin += 2U;
    size_t authority = begin;
    while (begin < bytes && uri[begin] != '/')
        ++begin;
    if (begin == bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (begin != authority && !LocalUriEqual(uri + authority, begin - authority, "localhost"))
        return UMI_STATUS_NOT_IMPLEMENTED;
    char decoded[UMI_PATH_CAPACITY], normal[UMI_PATH_CAPACITY];
    size_t used = 0U;
    for (size_t i = begin; i < bytes; ++i)
    {
        unsigned char value = (unsigned char)uri[i];
        int escaped = 0;
        if (value == '?' || value == '#' || value == '\\' || value <= 0x20U || value == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (value == '%')
        {
            if (bytes - i < 3U)
                return UMI_STATUS_INVALID_ARGUMENT;
            int high = LocalUriHex((unsigned char)uri[i + 1U]), low = LocalUriHex((unsigned char)uri[i + 2U]);
            if (high < 0 || low < 0)
                return UMI_STATUS_INVALID_ARGUMENT;
            value = (unsigned char)((unsigned)high * 16U + (unsigned)low);
            i += 2U;
            escaped = 1;
        }
        /* Escapes may represent a filename character, not a hidden path
         * separator or terminator that would change which resource is opened. */
        if (value < 0x20U || value == 0x7fU || (escaped && (value == '/' || value == '\\')))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (used + 1U >= sizeof(decoded))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        decoded[used++] = (char)value;
    }
    decoded[used] = '\0';
    if (!umi_document_utf8_validate((const unsigned char *)decoded, used, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (used >= 2U && decoded[0] == '/' && decoded[1] == '/')
        return UMI_STATUS_NOT_IMPLEMENTED;
#ifdef _WIN32
    /* A local Windows file URI has a drive letter after its leading slash.
     * UNC and device namespaces need an explicit separate access policy. */
    if (used < 4U || decoded[0] != '/' ||
        !((decoded[1] >= 'A' && decoded[1] <= 'Z') || (decoded[1] >= 'a' && decoded[1] <= 'z')) ||
        decoded[2] != ':' || decoded[3] != '/')
        return UMI_STATUS_INVALID_ARGUMENT;
    memmove(decoded, decoded + 1U, used);
    --used;
    for (size_t i = 2U; i < used; ++i)
        if (decoded[i] == ':')
            return UMI_STATUS_INVALID_ARGUMENT;
#endif
    UmiStatus status = umi_path_normalise(decoded, normal, sizeof(normal));
    if (status != UMI_STATUS_OK)
        return status;
    if (!umi_path_is_absolute(normal))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(normal);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_path, normal, length + 1U);
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiDocumentLocalFileUriToPath(const char *uri, char *out_path, size_t capacity)
{
    if (uri == NULL || out_path == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = 0U;
    while (bytes <= 8192U && uri[bytes] != '\0')
        ++bytes;
    if (bytes > 8192U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *colon = memchr(uri, ':', bytes);
    if (colon == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!LocalUriEqual(uri, (size_t)(colon - uri), "file"))
        return UMI_STATUS_NOT_IMPLEMENTED;
    size_t begin = (size_t)(colon - uri) + 1U;
    if (bytes - begin < 3U || uri[begin] != '/' || uri[begin + 1U] != '/')
        return UMI_STATUS_INVALID_ARGUMENT;
    begin += 2U;
    size_t authority = begin;
    while (begin < bytes && uri[begin] != '/')
        ++begin;
    if (begin == bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (begin != authority && !LocalUriEqual(uri + authority, begin - authority, "localhost"))
        return UMI_STATUS_NOT_IMPLEMENTED;
    char decoded[UMI_PATH_CAPACITY], normal[UMI_PATH_CAPACITY];
    size_t used = 0U;
    for (size_t i = begin; i < bytes; ++i)
    {
        unsigned char value = (unsigned char)uri[i];
        int escaped = 0;
        if (value == '?' || value == '#' || value == '\\' || value <= 0x20U || value == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (value == '%')
        {
            if (bytes - i < 3U)
                return UMI_STATUS_INVALID_ARGUMENT;
            int high = LocalUriHex((unsigned char)uri[i + 1U]), low = LocalUriHex((unsigned char)uri[i + 2U]);
            if (high < 0 || low < 0)
                return UMI_STATUS_INVALID_ARGUMENT;
            value = (unsigned char)((unsigned)high * 16U + (unsigned)low);
            i += 2U;
            escaped = 1;
        }
        /* Escapes may represent a filename character, not a hidden path
         * separator or terminator that would change which resource is opened. */
        if (value < 0x20U || value == 0x7fU || (escaped && (value == '/' || value == '\\')))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (used + 1U >= sizeof(decoded))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        decoded[used++] = (char)value;
    }
    decoded[used] = '\0';
    if (!umi_document_utf8_validate((const unsigned char *)decoded, used, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (used >= 2U && decoded[0] == '/' && decoded[1] == '/')
        return UMI_STATUS_NOT_IMPLEMENTED;
#ifdef _WIN32
    /* A local Windows file URI has a drive letter after its leading slash.
     * UNC and device namespaces need an explicit separate access policy. */
    if (used < 4U || decoded[0] != '/' ||
        !((decoded[1] >= 'A' && decoded[1] <= 'Z') || (decoded[1] >= 'a' && decoded[1] <= 'z')) ||
        decoded[2] != ':' || decoded[3] != '/')
        return UMI_STATUS_INVALID_ARGUMENT;
    memmove(decoded, decoded + 1U, used);
    --used;
    for (size_t i = 2U; i < used; ++i)
        if (decoded[i] == ':')
            return UMI_STATUS_INVALID_ARGUMENT;
#endif
    UmiStatus status = umi_path_normalise(decoded, normal, sizeof(normal));
    if (status != UMI_STATUS_OK)
        return status;
    if (!umi_path_is_absolute(normal))
        return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    /* Reuse the platform's syntax-only ordinary-file check. It does not open
     * or create anything. Device aliases and alternate streams must never be
     * treated as a source file merely because a server returned a file URI. */
    status = UmiOutputFileValidatePath(normal);
    if (status != UMI_STATUS_OK)
        return status;
#endif
    size_t length = strlen(normal);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_path, normal, length + 1U);
    return UMI_STATUS_OK;
}
