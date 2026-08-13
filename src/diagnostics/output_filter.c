#include "umicom/diagnostics/output_filter.h"

#include <ctype.h>
#include <string.h>

static int output_contains(const char *text, const char *pattern)
{
    size_t text_length;
    size_t pattern_length;
    size_t index;
    size_t offset;
    if (pattern == NULL || pattern[0] == '\0') return 1;
    if (text == NULL) return 0;
    text_length = strlen(text);
    pattern_length = strlen(pattern);
    for (index = 0U; index + pattern_length <= text_length; ++index) {
        for (offset = 0U; offset < pattern_length; ++offset) {
            if (tolower((unsigned char)text[index + offset]) !=
                tolower((unsigned char)pattern[offset])) break;
        }
        if (offset == pattern_length) return 1;
    }
    return 0;
}

void umi_output_filter_init(UmiOutputFilter *filter)
{
    if (filter != NULL) {
        (void)memset(filter, 0, sizeof(*filter));
        filter->stream_mask = UINT32_MAX;
    }
}

int umi_output_filter_matches(const UmiOutputFilter *filter,
                              const UmiOutputRecord *record)
{
    if (filter == NULL || record == NULL) return 0;
    if ((filter->stream_mask & (UINT32_C(1) << (unsigned int)record->stream)) == 0U) return 0;
    if (filter->minimum_sequence > record->sequence) return 0;
    if (filter->channel_id[0] != '\0' && strcmp(filter->channel_id, record->channel_id) != 0) return 0;
    if (filter->source[0] != '\0' && strcmp(filter->source, record->source) != 0) return 0;
    return output_contains(record->text, filter->text);
}

UmiStatus umi_output_filter_execute(const UmiOutputBuffer *buffer,
                                    const UmiOutputFilter *filter,
                                    UmiOutputFilterResult *out_result)
{
    size_t count;
    size_t index;
    if (buffer == NULL || filter == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));
    count = umi_output_buffer_count(buffer);
    for (index = 0U; index < count; ++index) {
        UmiOutputRecord record;
        if (umi_output_buffer_at(buffer, index, &record) == UMI_STATUS_OK &&
            umi_output_filter_matches(filter, &record)) {
            ++out_result->total_matches;
            if (out_result->count < UMI_OUTPUT_FILTER_RESULT_MAX) {
                out_result->items[out_result->count++] = record;
            } else {
                out_result->truncated = 1;
            }
        }
    }
    return UMI_STATUS_OK;
}
