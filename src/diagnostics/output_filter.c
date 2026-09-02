/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/output_filter.c
 *
 * PURPOSE:
 *   Implement the output filter behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/output_filter.h"

#include <ctype.h>
#include <string.h>

/* Provide the output contains operation used by this module and its client applications. */
static int output_contains(const char *text, const char *pattern)
{
    size_t text_length;
    size_t pattern_length;
    size_t index;
    size_t offset;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pattern == NULL || pattern[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    text_length = strlen(text);
    pattern_length = strlen(pattern);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index + pattern_length <= text_length; ++index) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U; offset < pattern_length; ++offset) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (tolower((unsigned char)text[index + offset]) !=
                tolower((unsigned char)pattern[offset])) break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (offset == pattern_length) return 1;
    }
    return 0;
}

/*
 * Initialise output filter from caller-provided values so later operations receive a known
 * state.
 */
void umi_output_filter_init(UmiOutputFilter *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter != NULL) {
        (void)memset(filter, 0, sizeof(*filter));
        filter->stream_mask = UINT32_MAX;
    }
}

/*
 * Provide the output filter matches operation used by this module and its client
 * applications.
 */
int umi_output_filter_matches(const UmiOutputFilter *filter,
                              const UmiOutputRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL || record == NULL) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((filter->stream_mask & (UINT32_C(1) << (unsigned int)record->stream)) == 0U) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (filter->minimum_sequence > record->sequence) return 0;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (filter->channel_id[0] != '\0' && strcmp(filter->channel_id, record->channel_id) != 0) return 0;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (filter->source[0] != '\0' && strcmp(filter->source, record->source) != 0) return 0;
    return output_contains(record->text, filter->text);
}

/*
 * Perform output filter through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_output_filter_execute(const UmiOutputBuffer *buffer,
                                    const UmiOutputFilter *filter,
                                    UmiOutputFilterResult *out_result)
{
    size_t count;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || filter == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    count = umi_output_buffer_count(buffer);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiOutputRecord record;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_output_buffer_at(buffer, index, &record) == UMI_STATUS_OK &&
            umi_output_filter_matches(filter, &record)) {
            ++out_result->total_matches;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out_result->count < UMI_OUTPUT_FILTER_RESULT_MAX) {
                out_result->items[out_result->count++] = record;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                out_result->truncated = 1;
            }
        }
    }
    return UMI_STATUS_OK;
}
