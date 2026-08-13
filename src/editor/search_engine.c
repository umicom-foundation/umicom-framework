/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/search_engine.c
 *
 * PURPOSE:
 *   Implement allocation-free literal search with explicit ASCII case and
 *   identifier-boundary behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/search_engine.h"

#include <ctype.h>
#include <string.h>

static unsigned char ascii_lower(unsigned char value)
{
    return value >= (unsigned char)'A' && value <= (unsigned char)'Z'
        ? (unsigned char)(value + ((unsigned char)'a' - (unsigned char)'A'))
        : value;
}

static int needle_has_upper(const char *needle, size_t count)
{
    size_t index;
    for (index = 0U; index < count; ++index) {
        unsigned char value = (unsigned char)needle[index];
        if (value >= (unsigned char)'A' && value <= (unsigned char)'Z') return 1;
    }
    return 0;
}

static int byte_equal(unsigned char left,
                      unsigned char right,
                      int case_sensitive)
{
    return case_sensitive
        ? left == right
        : ascii_lower(left) == ascii_lower(right);
}

static int is_word_byte(unsigned char value)
{
    return (value >= (unsigned char)'a' && value <= (unsigned char)'z') ||
           (value >= (unsigned char)'A' && value <= (unsigned char)'Z') ||
           (value >= (unsigned char)'0' && value <= (unsigned char)'9') ||
           value == (unsigned char)'_' || value >= 0x80U;
}

static int is_match(const char *haystack,
                    size_t offset,
                    const char *needle,
                    size_t needle_count,
                    int case_sensitive)
{
    size_t index;
    for (index = 0U; index < needle_count; ++index) {
        if (!byte_equal((unsigned char)haystack[offset + index],
                        (unsigned char)needle[index],
                        case_sensitive)) {
            return 0;
        }
    }
    return 1;
}

UmiStatus umi_editor_search_literal(const char *haystack,
                                     size_t haystack_byte_count,
                                     const char *needle,
                                     size_t needle_byte_count,
                                     const UmiEditorSearchOptions *options,
                                     UmiEditorSearchResults *out_results)
{
    UmiEditorSearchOptions effective = {
        UMI_EDITOR_SEARCH_CASE_SENSITIVE, 0, 0,
        UMI_EDITOR_SEARCH_MATCH_CAPACITY
    };
    size_t offset;
    size_t limit;
    int case_sensitive;

    if ((haystack == NULL && haystack_byte_count > 0U) ||
        needle == NULL || needle_byte_count == 0U || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (options != NULL) effective = *options;
    if (effective.case_mode < UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        effective.case_mode > UMI_EDITOR_SEARCH_CASE_SMART) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (effective.maximum_matches == 0U ||
        effective.maximum_matches > UMI_EDITOR_SEARCH_MATCH_CAPACITY) {
        effective.maximum_matches = UMI_EDITOR_SEARCH_MATCH_CAPACITY;
    }

    (void)memset(out_results, 0, sizeof(*out_results));
    if (needle_byte_count > haystack_byte_count) return UMI_STATUS_OK;
    case_sensitive = effective.case_mode == UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        (effective.case_mode == UMI_EDITOR_SEARCH_CASE_SMART &&
         needle_has_upper(needle, needle_byte_count));
    limit = haystack_byte_count - needle_byte_count;

    offset = 0U;
    while (offset <= limit) {
        if (is_match(haystack, offset, needle, needle_byte_count, case_sensitive)) {
            int begins_word = offset == 0U ||
                !is_word_byte((unsigned char)haystack[offset - 1U]);
            int ends_word = offset + needle_byte_count == haystack_byte_count ||
                !is_word_byte((unsigned char)haystack[offset + needle_byte_count]);
            if (!effective.whole_word || (begins_word && ends_word)) {
                if (out_results->count >= effective.maximum_matches) {
                    out_results->truncated = 1;
                    break;
                }
                out_results->matches[out_results->count].offset = offset;
                out_results->matches[out_results->count].byte_count =
                    needle_byte_count;
                out_results->count += 1U;
                offset += effective.allow_overlapping ? 1U : needle_byte_count;
                continue;
            }
        }
        offset += 1U;
    }
    return UMI_STATUS_OK;
}
