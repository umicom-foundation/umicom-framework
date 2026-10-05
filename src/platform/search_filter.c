/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/search_filter.c
 * PURPOSE: Validate and match bounded workspace-relative search patterns without recursion.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/search_filter.h"
#include <string.h>

/* Byte limits also bound wildcard work. Reject unsupported pattern syntax so
 * a spelling resembling a shell glob is never silently interpreted differently. */
static UmiStatus FilterListCopy(const char *input, char *output)
{
    if (input == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U, written = 0U, count = 0U;
    while (length < UMI_SEARCH_FILTER_TEXT_CAPACITY && input[length] != '\0') ++length;
    if (length == UMI_SEARCH_FILTER_TEXT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t first = 0U, last = length;
    while (first < last && (input[first] == ' ' || input[first] == '\t')) ++first;
    while (last > first && (input[last - 1U] == ' ' || input[last - 1U] == '\t')) --last;
    if (first == last) { output[0] = '\0'; return UMI_STATUS_OK; }
    while (first < last) {
        size_t separator = first;
        while (separator < last && input[separator] != ';') ++separator;
        size_t start = first, end = separator;
        while (start < end && (input[start] == ' ' || input[start] == '\t')) ++start;
        while (end > start && (input[end - 1U] == ' ' || input[end - 1U] == '\t')) --end;
        if (start == end || input[start] == '/' || input[start] == '\\') return UMI_STATUS_INVALID_ARGUMENT;
        if (++count > UMI_SEARCH_FILTER_MAX_PATTERNS) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (written != 0U) output[written++] = ';';
        for (size_t index = start; index < end; ++index) {
            unsigned char byte = (unsigned char)input[index];
            if (byte < 32U || byte == 127U || byte == ':' || byte == '[' || byte == ']' ||
                byte == '{' || byte == '}') return UMI_STATUS_INVALID_ARGUMENT;
            output[written++] = byte == '\\' ? '/' : (char)byte;
        }
        if (separator == last) break;
        first = separator + 1U;
        if (first == last) return UMI_STATUS_INVALID_ARGUMENT;
    }
    output[written] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus UmiSearchPathFilterInit(const char *include_patterns,
    const char *exclude_patterns, UmiSearchPathFilter *out_filter)
{
    if (out_filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSearchPathFilter candidate = {0};
    UmiStatus status = FilterListCopy(include_patterns, candidate.include_patterns);
    if (status == UMI_STATUS_OK) status = FilterListCopy(exclude_patterns, candidate.exclude_patterns);
    if (status == UMI_STATUS_OK) *out_filter = candidate;
    return status;
}

/* Keep one remembered star rather than recursively trying every split. Failed
 * suffixes advance through the bounded path, so malformed input cannot grow
 * a recursion stack. Paths have already been normalised to forward slashes. */
static int PatternMatches(const char *pattern, size_t pattern_size, const char *path, size_t path_size)
{
    size_t pattern_at = 0U, path_at = 0U, star = SIZE_MAX, retry = 0U;
    while (path_at < path_size) {
        if (pattern_at < pattern_size && (pattern[pattern_at] == '?' || (pattern[pattern_at] != '*' && pattern[pattern_at] == path[path_at]))) {
            ++pattern_at; ++path_at;
        } else if (pattern_at < pattern_size && pattern[pattern_at] == '*') {
            star = ++pattern_at; retry = path_at;
        } else if (star != SIZE_MAX) {
            pattern_at = star; path_at = ++retry;
        } else return 0;
    }
    while (pattern_at < pattern_size && pattern[pattern_at] == '*') ++pattern_at;
    return pattern_at == pattern_size;
}

static int ListMatches(const char *list, const char *path, size_t path_size, size_t basename)
{
    size_t start = 0U;
    while (list[start] != '\0') {
        size_t end = start; int has_slash = 0;
        while (list[end] != '\0' && list[end] != ';') {
            if (list[end] == '/') has_slash = 1;
            ++end;
        }
        size_t offset = has_slash ? 0U : basename;
        if (PatternMatches(list + start, end - start, path + offset, path_size - offset)) return 1;
        if (list[end] == '\0') break;
        start = end + 1U;
    }
    return 0;
}

UmiStatus UmiSearchPathFilterMatch(const UmiSearchPathFilter *filter,
    const char *relative_path, int *out_matches)
{
    if (filter == NULL || relative_path == NULL || out_matches == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiSearchPathFilter candidate;
    UmiStatus status = UmiSearchPathFilterInit(filter->include_patterns, filter->exclude_patterns, &candidate);
    if (status != UMI_STATUS_OK) return status;
    char path[UMI_PATH_CAPACITY]; size_t length = 0U, basename = 0U;
    while (length < sizeof(path) && relative_path[length] != '\0') ++length;
    if (length == sizeof(path)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length == 0U || relative_path[0] == '/' || relative_path[0] == '\\') return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < length; ++index) {
        unsigned char byte = (unsigned char)relative_path[index];
        if (byte < 32U || byte == 127U || byte == ':') return UMI_STATUS_INVALID_ARGUMENT;
        path[index] = byte == '\\' ? '/' : (char)byte;
        if (path[index] == '/') basename = index + 1U;
    }
    path[length] = '\0';
    if (basename == length) return UMI_STATUS_INVALID_ARGUMENT;
    *out_matches = (candidate.include_patterns[0] == '\0' ||
        ListMatches(candidate.include_patterns, path, length, basename)) &&
        !ListMatches(candidate.exclude_patterns, path, length, basename);
    return UMI_STATUS_OK;
}
