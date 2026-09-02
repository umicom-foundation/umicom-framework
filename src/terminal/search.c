/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/search.c
 *
 * PURPOSE:
 *   Implement deterministic transcript search over immutable line snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/search.h"

#include <ctype.h>
#include <string.h>

/* Provide the is word operation used by this module and its client applications. */
static int is_word(unsigned char value)
{
    return isalnum(value) || value == (unsigned char)'_';
}

/* Provide the byte equal operation used by this module and its client applications. */
static int byte_equal(unsigned char left,
                      unsigned char right,
                      int case_sensitive)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (case_sensitive) return left == right;
    return tolower(left) == tolower(right);
}

/* Provide the find text operation used by this module and its client applications. */
static const char *find_text(const char *text,
                             const char *query,
                             int case_sensitive,
                             int whole_word)
{
    size_t text_length = strlen(text);
    size_t query_length = strlen(query);
    size_t offset;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_length == 0U || query_length > text_length) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (offset = 0U; offset + query_length <= text_length; ++offset) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < query_length; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!byte_equal((unsigned char)text[offset + index],
                            (unsigned char)query[index], case_sensitive)) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index != query_length) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (whole_word &&
            ((offset > 0U && is_word((unsigned char)text[offset - 1U])) ||
             (offset + query_length < text_length &&
              is_word((unsigned char)text[offset + query_length])))) {
            continue;
        }
        return text + offset;
    }
    return NULL;
}

/*
 * Initialise terminal search query from caller-provided values so later operations receive
 * a known state.
 */
void umi_terminal_search_query_init(UmiTerminalSearchQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    query->limit = UMI_TERMINAL_SEARCH_MAX_RESULTS;
}

/*
 * Provide the terminal search transcript operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_search_transcript(
    const UmiTerminalTranscript *transcript,
    const UmiTerminalSearchQuery *query,
    UmiTerminalSearchResult *out_result)
{
    size_t count;
    size_t index;
    size_t limit;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transcript == NULL || query == NULL || out_result == NULL ||
        query->text[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    limit = query->limit == 0U ? UMI_TERMINAL_SEARCH_MAX_RESULTS : query->limit;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit > UMI_TERMINAL_SEARCH_MAX_RESULTS) {
        limit = UMI_TERMINAL_SEARCH_MAX_RESULTS;
    }
    count = umi_terminal_transcript_count(transcript);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiTerminalTranscriptLine line;
        const char *match;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_terminal_transcript_at(transcript, index, &line) != UMI_STATUS_OK) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (line.sequence <= query->after_sequence ||
            (query->use_stream_filter && line.stream != query->stream)) continue;
        match = find_text(line.text, query->text,
                          query->case_sensitive, query->whole_word);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (match == NULL) continue;
        out_result->total_matches += 1U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_result->count < limit) {
            UmiTerminalSearchMatch *result =
                &out_result->matches[out_result->count++];
            result->sequence = line.sequence;
            result->line_index = index;
            result->byte_offset = (size_t)(match - line.text);
            result->byte_length = strlen(query->text);
            result->stream = line.stream;
            (void)memcpy(result->preview, line.text, strlen(line.text) + 1U);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_result->truncated = 1;
        }
    }
    return UMI_STATUS_OK;
}
