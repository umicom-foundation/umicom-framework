/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/search.c
 *
 * PURPOSE:
 *   Implement deterministic transcript search over immutable line snapshots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/search.h"

#include <ctype.h>
#include <string.h>

static int is_word(unsigned char value)
{
    return isalnum(value) || value == (unsigned char)'_';
}

static int byte_equal(unsigned char left,
                      unsigned char right,
                      int case_sensitive)
{
    if (case_sensitive) return left == right;
    return tolower(left) == tolower(right);
}

static const char *find_text(const char *text,
                             const char *query,
                             int case_sensitive,
                             int whole_word)
{
    size_t text_length = strlen(text);
    size_t query_length = strlen(query);
    size_t offset;
    if (query_length == 0U || query_length > text_length) return NULL;
    for (offset = 0U; offset + query_length <= text_length; ++offset) {
        size_t index;
        for (index = 0U; index < query_length; ++index) {
            if (!byte_equal((unsigned char)text[offset + index],
                            (unsigned char)query[index], case_sensitive)) break;
        }
        if (index != query_length) continue;
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

void umi_terminal_search_query_init(UmiTerminalSearchQuery *query)
{
    if (query == NULL) return;
    (void)memset(query, 0, sizeof(*query));
    query->limit = UMI_TERMINAL_SEARCH_MAX_RESULTS;
}

UmiStatus umi_terminal_search_transcript(
    const UmiTerminalTranscript *transcript,
    const UmiTerminalSearchQuery *query,
    UmiTerminalSearchResult *out_result)
{
    size_t count;
    size_t index;
    size_t limit;
    if (transcript == NULL || query == NULL || out_result == NULL ||
        query->text[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    limit = query->limit == 0U ? UMI_TERMINAL_SEARCH_MAX_RESULTS : query->limit;
    if (limit > UMI_TERMINAL_SEARCH_MAX_RESULTS) {
        limit = UMI_TERMINAL_SEARCH_MAX_RESULTS;
    }
    count = umi_terminal_transcript_count(transcript);
    for (index = 0U; index < count; ++index) {
        UmiTerminalTranscriptLine line;
        const char *match;
        if (umi_terminal_transcript_at(transcript, index, &line) != UMI_STATUS_OK) {
            continue;
        }
        if (line.sequence <= query->after_sequence ||
            (query->use_stream_filter && line.stream != query->stream)) continue;
        match = find_text(line.text, query->text,
                          query->case_sensitive, query->whole_word);
        if (match == NULL) continue;
        out_result->total_matches += 1U;
        if (out_result->count < limit) {
            UmiTerminalSearchMatch *result =
                &out_result->matches[out_result->count++];
            result->sequence = line.sequence;
            result->line_index = index;
            result->byte_offset = (size_t)(match - line.text);
            result->byte_length = strlen(query->text);
            result->stream = line.stream;
            (void)memcpy(result->preview, line.text, strlen(line.text) + 1U);
        } else {
            out_result->truncated = 1;
        }
    }
    return UMI_STATUS_OK;
}
