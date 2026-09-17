/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/search_engine.c
 *
 * PURPOSE:
 *   Implement allocation-free literal search with explicit ASCII case and
 *   identifier-boundary behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/search_engine.h"

#include <ctype.h>
#include <string.h>

/* Provide the ascii lower operation used by this module and its client applications. */
static unsigned char ascii_lower(unsigned char value)
{
    return value >= (unsigned char)'A' && value <= (unsigned char)'Z'
        ? (unsigned char)(value + ((unsigned char)'a' - (unsigned char)'A'))
        : value;
}

/* Provide the needle has upper operation used by this module and its client applications. */
static int needle_has_upper(const char *needle, size_t count)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        unsigned char value = (unsigned char)needle[index];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (value >= (unsigned char)'A' && value <= (unsigned char)'Z') return 1;
    }
    return 0;
}

/* Provide the byte equal operation used by this module and its client applications. */
static int byte_equal(unsigned char left,
                      unsigned char right,
                      int case_sensitive)
{
    return case_sensitive
        ? left == right
        : ascii_lower(left) == ascii_lower(right);
}

/* Provide the is word byte operation used by this module and its client applications. */
static int is_word_byte(unsigned char value)
{
    return (value >= (unsigned char)'a' && value <= (unsigned char)'z') ||
           (value >= (unsigned char)'A' && value <= (unsigned char)'Z') ||
           (value >= (unsigned char)'0' && value <= (unsigned char)'9') ||
           value == (unsigned char)'_' || value >= 0x80U;
}

/* Provide the is match operation used by this module and its client applications. */
static int is_match(const char *haystack,
                    size_t offset,
                    const char *needle,
                    size_t needle_count,
                    int case_sensitive)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < needle_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!byte_equal((unsigned char)haystack[offset + index],
                        (unsigned char)needle[index],
                        case_sensitive)) {
            return 0;
        }
    }
    return 1;
}

/*
 * Provide the editor search literal operation used by this module and its client
 * applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((haystack == NULL && haystack_byte_count > 0U) ||
        needle == NULL || needle_byte_count == 0U || out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options != NULL) effective = *options;
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.case_mode < UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        effective.case_mode > UMI_EDITOR_SEARCH_CASE_SMART) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (effective.maximum_matches == 0U ||
        effective.maximum_matches > UMI_EDITOR_SEARCH_MATCH_CAPACITY) {
        effective.maximum_matches = UMI_EDITOR_SEARCH_MATCH_CAPACITY;
    }

    (void)memset(out_results, 0, sizeof(*out_results));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (needle_byte_count > haystack_byte_count) return UMI_STATUS_OK;
    case_sensitive = effective.case_mode == UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        (effective.case_mode == UMI_EDITOR_SEARCH_CASE_SMART &&
         needle_has_upper(needle, needle_byte_count));
    limit = haystack_byte_count - needle_byte_count;

    offset = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (offset <= limit) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (is_match(haystack, offset, needle, needle_byte_count, case_sensitive)) {
            int begins_word = offset == 0U ||
                !is_word_byte((unsigned char)haystack[offset - 1U]);
            int ends_word = offset + needle_byte_count == haystack_byte_count ||
                !is_word_byte((unsigned char)haystack[offset + needle_byte_count]);
            /* Apply this branch only when its contract condition is satisfied. */
            if (!effective.whole_word || (begins_word && ends_word)) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/* Keep whole-word and case behaviour identical for result lists, navigation
 * and replacement. ASCII case folding leaves UTF-8 bytes unchanged. */
static int SearchMatchAt(const char *text, size_t length, size_t offset,
    const char *needle, size_t needleLength, int sensitive, int wholeWord)
{
    if (!is_match(text, offset, needle, needleLength, sensitive)) return 0;
    return !wholeWord || ((offset == 0U || !is_word_byte((unsigned char)text[offset - 1U])) &&
        (offset + needleLength == length || !is_word_byte((unsigned char)text[offset + needleLength])));
}

/* Navigation does not materialise a bounded result list, so the next match
 * remains reachable even when a document contains more than 4,096 matches. */
UmiStatus UmiEditorSearchNavigate(const char *haystack, size_t haystackBytes,
    const char *needle, size_t needleBytes, const UmiEditorSearchOptions *options,
    size_t start, int backwards, int wrap, UmiEditorSearchMatch *outMatch,
    int *outWrapped)
{
    UmiEditorSearchOptions effective = {UMI_EDITOR_SEARCH_CASE_SENSITIVE, 0, 0, 0};
    size_t selected = (size_t)-1;
    size_t fallback = (size_t)-1;
    if ((haystack == NULL && haystackBytes != 0U) || needle == NULL ||
        needleBytes == 0U || start > haystackBytes || outMatch == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (options != NULL) effective = *options;
    if (effective.case_mode < UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        effective.case_mode > UMI_EDITOR_SEARCH_CASE_SMART) return UMI_STATUS_INVALID_ARGUMENT;
    if (needleBytes > haystackBytes) return UMI_STATUS_NOT_FOUND;
    int sensitive = effective.case_mode == UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        (effective.case_mode == UMI_EDITOR_SEARCH_CASE_SMART && needle_has_upper(needle, needleBytes));
    size_t limit = haystackBytes - needleBytes;
    for (size_t offset = 0U; offset <= limit; ++offset) {
        if (!SearchMatchAt(haystack, haystackBytes, offset, needle, needleBytes, sensitive, effective.whole_word)) continue;
        if (backwards) {
            fallback = offset;
            if (offset < start) selected = offset;
        } else {
            if (fallback == (size_t)-1) fallback = offset;
            if (offset >= start) { selected = offset; break; }
        }
    }
    int wrapped = selected == (size_t)-1;
    if (wrapped) {
        if (!wrap || fallback == (size_t)-1) return UMI_STATUS_NOT_FOUND;
        selected = fallback;
    }
    outMatch->offset = selected;
    outMatch->byte_count = needleBytes;
    if (outWrapped != NULL) *outWrapped = wrapped;
    return UMI_STATUS_OK;
}

/* Count the original matches once before allocating a complete replacement.
 * The calculation is shared with the bounded-output API below. */
UmiStatus UmiEditorSearchReplaceAllSize(const char *text, size_t textBytes,
    const char *needle, size_t needleBytes, size_t replacementBytes,
    const UmiEditorSearchOptions *options, size_t *outBytes, size_t *outCount)
{
    UmiEditorSearchOptions effective = {UMI_EDITOR_SEARCH_CASE_SENSITIVE, 0, 0, 0};
    size_t resultLength = textBytes;
    size_t count = 0U;
    if ((text == NULL && textBytes != 0U) || needle == NULL || needleBytes == 0U ||
        outBytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (options != NULL) effective = *options;
    if (effective.case_mode < UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        effective.case_mode > UMI_EDITOR_SEARCH_CASE_SMART) return UMI_STATUS_INVALID_ARGUMENT;
    int sensitive = effective.case_mode == UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        (effective.case_mode == UMI_EDITOR_SEARCH_CASE_SMART && needle_has_upper(needle, needleBytes));
    for (size_t offset = 0U; offset < textBytes;) {
        if (needleBytes <= textBytes - offset &&
            SearchMatchAt(text, textBytes, offset, needle, needleBytes, sensitive, effective.whole_word)) {
            if (replacementBytes > needleBytes) {
                size_t growth = replacementBytes - needleBytes;
                if (growth > (size_t)-1 - resultLength) return UMI_STATUS_CAPACITY_EXCEEDED;
                resultLength += growth;
            } else resultLength -= needleBytes - replacementBytes;
            ++count;
            offset += needleBytes;
        } else ++offset;
    }
    *outBytes = resultLength;
    if (outCount != NULL) *outCount = count;
    return UMI_STATUS_OK;
}

/* The first pass proves the complete result fits. No caller-visible byte is
 * written until that pass succeeds; replacement never searches its own output. */
UmiStatus UmiEditorSearchReplaceAll(const char *text, size_t textBytes,
    const char *needle, size_t needleBytes, const char *replacement,
    size_t replacementBytes, const UmiEditorSearchOptions *options,
    char *outText, size_t capacity, size_t *outCount)
{
    UmiEditorSearchOptions effective = {UMI_EDITOR_SEARCH_CASE_SENSITIVE, 0, 0, 0};
    size_t resultLength = 0U;
    size_t count = 0U;
    if ((replacement == NULL && replacementBytes != 0U) || outText == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiEditorSearchReplaceAllSize(text, textBytes, needle,
        needleBytes, replacementBytes, options, &resultLength, &count);
    if (status != UMI_STATUS_OK) return status;
    if (resultLength >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (options != NULL) effective = *options;
    int sensitive = effective.case_mode == UMI_EDITOR_SEARCH_CASE_SENSITIVE ||
        (effective.case_mode == UMI_EDITOR_SEARCH_CASE_SMART && needle_has_upper(needle, needleBytes));
    size_t used = 0U;
    for (size_t offset = 0U; offset < textBytes;) {
        if (needleBytes <= textBytes - offset &&
            SearchMatchAt(text, textBytes, offset, needle, needleBytes, sensitive, effective.whole_word)) {
            if (replacementBytes > 0U) memcpy(outText + used, replacement, replacementBytes);
            used += replacementBytes;
            offset += needleBytes;
        } else outText[used++] = text[offset++];
    }
    outText[used] = '\0';
    if (outCount != NULL) *outCount = count;
    return UMI_STATUS_OK;
}
