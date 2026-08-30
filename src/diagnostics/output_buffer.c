/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/output_buffer.c
 *
 * PURPOSE:
 *   Implement the output buffer behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/output_buffer.h"

#include <stdlib.h>
#include <string.h>

#include "diagnostic_lock.h"

struct UmiOutputBuffer {
    UmiOutputRecord *items;
    size_t capacity;
    size_t count;
    size_t head;
    uint64_t next_sequence;
    uint64_t total_received;
    uint64_t overwritten_count;
    uint64_t revision;
    UmiDiagnosticLock lock;
};

static size_t output_index(const UmiOutputBuffer *buffer, size_t logical)
{
    return (buffer->head + logical) % buffer->capacity;
}

UmiOutputBufferConfig umi_output_buffer_config_default(void)
{
    UmiOutputBufferConfig config = { UMI_OUTPUT_BUFFER_DEFAULT_CAPACITY };
    return config;
}

UmiStatus umi_output_buffer_create(const UmiOutputBufferConfig *config,
                                   UmiOutputBuffer **out_buffer)
{
    UmiOutputBufferConfig effective;
    UmiOutputBuffer *buffer;
    if (out_buffer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_buffer = NULL;
    effective = config != NULL ? *config : umi_output_buffer_config_default();
    if (effective.capacity == 0U || effective.capacity > UMI_OUTPUT_BUFFER_MAX_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    buffer = (UmiOutputBuffer *)calloc(1U, sizeof(*buffer));
    if (buffer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    buffer->items = (UmiOutputRecord *)calloc(effective.capacity, sizeof(*buffer->items));
    if (buffer->items == NULL) {
        free(buffer);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    buffer->capacity = effective.capacity;
    buffer->next_sequence = 1U;
    umi_diagnostic_lock_init(&buffer->lock);
    *out_buffer = buffer;
    return UMI_STATUS_OK;
}

void umi_output_buffer_destroy(UmiOutputBuffer *buffer)
{
    if (buffer != NULL) {
        free(buffer->items);
        free(buffer);
    }
}

void umi_output_buffer_clear(UmiOutputBuffer *buffer)
{
    if (buffer == NULL) return;
    umi_diagnostic_lock_acquire(&buffer->lock);
    (void)memset(buffer->items, 0, buffer->capacity * sizeof(*buffer->items));
    buffer->count = 0U;
    buffer->head = 0U;
    ++buffer->revision;
    umi_diagnostic_lock_release(&buffer->lock);
}

UmiStatus umi_output_buffer_append(UmiOutputBuffer *buffer,
                                   const UmiOutputRecord *record)
{
    UmiOutputRecord stored;
    size_t destination;
    if (buffer == NULL || record == NULL || record->channel_id[0] == '\0' ||
        record->stream < UMI_OUTPUT_STREAM_STANDARD ||
        record->stream > UMI_OUTPUT_STREAM_DEBUG) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    stored = *record;
    stored.channel_id[sizeof(stored.channel_id) - 1U] = '\0';
    stored.channel_name[sizeof(stored.channel_name) - 1U] = '\0';
    stored.source[sizeof(stored.source) - 1U] = '\0';
    stored.text[sizeof(stored.text) - 1U] = '\0';
    umi_diagnostic_lock_acquire(&buffer->lock);
    if (buffer->count < buffer->capacity) {
        destination = output_index(buffer, buffer->count);
        ++buffer->count;
    } else {
        destination = buffer->head;
        buffer->head = (buffer->head + 1U) % buffer->capacity;
        ++buffer->overwritten_count;
    }
    stored.sequence = buffer->next_sequence++;
    stored.revision = ++buffer->revision;
    buffer->items[destination] = stored;
    ++buffer->total_received;
    umi_diagnostic_lock_release(&buffer->lock);
    return UMI_STATUS_OK;
}

UmiStatus umi_output_buffer_at(const UmiOutputBuffer *buffer,
                               size_t chronological_index,
                               UmiOutputRecord *out_record)
{
    if (buffer == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&buffer->lock);
    if (chronological_index >= buffer->count) {
        umi_diagnostic_lock_release(&buffer->lock);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_record = buffer->items[output_index(buffer, chronological_index)];
    umi_diagnostic_lock_release(&buffer->lock);
    return UMI_STATUS_OK;
}

UmiStatus umi_output_buffer_summary(const UmiOutputBuffer *buffer,
                                    UmiOutputBufferSummary *out_summary)
{
    size_t index;
    if (buffer == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_summary, 0, sizeof(*out_summary));
    umi_diagnostic_lock_acquire(&buffer->lock);
    out_summary->retained_count = buffer->count;
    out_summary->total_received = buffer->total_received;
    out_summary->overwritten_count = buffer->overwritten_count;
    out_summary->revision = buffer->revision;
    for (index = 0U; index < buffer->count; ++index) {
        const UmiOutputRecord *item = &buffer->items[output_index(buffer, index)];
        size_t prior;
        int first = 1;
        ++out_summary->by_stream[(size_t)item->stream];
        for (prior = 0U; prior < index; ++prior) {
            if (strcmp(item->channel_id,
                       buffer->items[output_index(buffer, prior)].channel_id) == 0) {
                first = 0;
                break;
            }
        }
        if (first != 0) ++out_summary->channel_count;
    }
    umi_diagnostic_lock_release(&buffer->lock);
    return UMI_STATUS_OK;
}

size_t umi_output_buffer_count(const UmiOutputBuffer *buffer)
{
    size_t count = 0U;
    if (buffer != NULL) {
        umi_diagnostic_lock_acquire(&buffer->lock);
        count = buffer->count;
        umi_diagnostic_lock_release(&buffer->lock);
    }
    return count;
}

uint64_t umi_output_buffer_revision(const UmiOutputBuffer *buffer)
{
    uint64_t revision = 0U;
    if (buffer != NULL) {
        umi_diagnostic_lock_acquire(&buffer->lock);
        revision = buffer->revision;
        umi_diagnostic_lock_release(&buffer->lock);
    }
    return revision;
}
