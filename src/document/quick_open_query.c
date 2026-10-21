/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/quick_open_query.c
 * PURPOSE: Reuse document position rules without mistaking drive paths for lines.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/quick_open_query.h"
#include <string.h>
static int QuickOpenDrivePrefix(const char *text, size_t bytes, size_t colon)
{
    return colon == 1U && bytes >= 3U &&
        ((text[0] >= 'A' && text[0] <= 'Z') || (text[0] >= 'a' && text[0] <= 'z')) &&
        (text[2] == '/' || text[2] == '\\');
}
UmiStatus UmiQuickOpenQueryParse(const char *text, size_t byteCount, UmiQuickOpenQuery *outQuery)
{
    if (text == NULL || outQuery == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (byteCount > UMI_QUICK_OPEN_QUERY_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\0', byteCount) != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiQuickOpenQuery candidate = {0};
    size_t begin = 0U, end = byteCount, suffix = byteCount;
    if (byteCount != 0U && text[0] == '"') {
        const char *closing = memchr(text + 1U, '"', byteCount - 1U);
        if (closing == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        begin = 1U; end = (size_t)(closing - text);
        if (end == begin) return UMI_STATUS_INVALID_ARGUMENT;
        if (end + 1U != byteCount) {
            if (text[end + 1U] != ':') return UMI_STATUS_INVALID_ARGUMENT;
            suffix = end + 2U;
        }
    } else {
        size_t lastSeparator = 0U;
        for (size_t i = 0U; i < byteCount; ++i) {
            if (text[i] == '"') return UMI_STATUS_INVALID_ARGUMENT;
            if (text[i] == '/' || text[i] == '\\') lastSeparator = i + 1U;
        }
        for (size_t i = lastSeparator; i < byteCount; ++i) {
            if (text[i] == ':' && !QuickOpenDrivePrefix(text, byteCount, i)) {
                end = i; suffix = i + 1U; break;
            }
        }
    }
    /* A trailing delimiter differs from a query with no delimiter. Keep that
     * distinction so incomplete typing never silently opens a literal match. */
    int positioned = end != byteCount && !(begin == 1U && end + 1U == byteCount);
    if (positioned) {
        if (end == begin || suffix == byteCount) return UMI_STATUS_INVALID_ARGUMENT;
        UmiStatus status = UmiDocumentLocationParse(text + suffix, byteCount - suffix, &candidate.location);
        if (status != UMI_STATUS_OK) return status;
        candidate.has_location = 1;
    }
    memcpy(candidate.filter, text + begin, end - begin);
    candidate.filter[end - begin] = '\0';
    *outQuery = candidate;
    return UMI_STATUS_OK;
}
