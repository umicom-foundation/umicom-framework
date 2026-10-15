/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/output.c
 *
 * PURPOSE:
 *   Implement the output behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/output.h"
#include "../base/value_construction_internal.h"

#include <string.h>

/* Provide the copy output text operation used by this module and its client applications. */
static int copy_output_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return 0;
    source = source != NULL ? source : "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return 0;
    (void)memcpy(destination, source, length + 1U);
    return 1;
}

/*
 * Initialise output record from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_output_record_init(UmiOutputRecord *record,
                                 const char *channel_id,
                                 const char *channel_name,
                                 const char *source,
                                 UmiOutputStream stream,
                                 const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || channel_id == NULL || channel_id[0] == '\0' ||
        text == NULL || stream < UMI_OUTPUT_STREAM_STANDARD ||
        stream > UMI_OUTPUT_STREAM_DEBUG) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(record, 0, sizeof(*record));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_output_text(record->channel_id, sizeof(record->channel_id), channel_id) ||
        !copy_output_text(record->channel_name, sizeof(record->channel_name), channel_name) ||
        !copy_output_text(record->source, sizeof(record->source), source) ||
        !copy_output_text(record->text, sizeof(record->text), text)) {
        (void)memset(record, 0, sizeof(*record));
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    record->stream = stream;
    record->complete_line = 1;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_output_record_init_build_value(UmiOutputRecord *record,
                                 const char *channel_id,
                                 const char *channel_name,
                                 const char *source,
                                 UmiOutputStream stream,
                                 const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || channel_id == NULL || channel_id[0] == '\0' ||
        text == NULL || stream < UMI_OUTPUT_STREAM_STANDARD ||
        stream > UMI_OUTPUT_STREAM_DEBUG) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(record, 0, sizeof(*record));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_output_text(record->channel_id, sizeof(record->channel_id), channel_id) ||
        !copy_output_text(record->channel_name, sizeof(record->channel_name), channel_name) ||
        !copy_output_text(record->source, sizeof(record->source), source) ||
        !copy_output_text(record->text, sizeof(record->text), text)) {
        (void)memset(record, 0, sizeof(*record));
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    record->stream = stream;
    record->complete_line = 1;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_output_record_init, UmiOutputRecord, record,
    (UmiOutputRecord *record, const char *channel_id, const char *channel_name, const char *source, UmiOutputStream stream, const char *text),
    umi_output_record_init_build_value(candidate, channel_id, channel_name, source, stream, text))


/*
 * Provide the output stream text operation used by this module and its client
 * applications.
 */
const char *umi_output_stream_text(UmiOutputStream stream)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (stream) {
        case UMI_OUTPUT_STREAM_STANDARD: return "STANDARD";
        case UMI_OUTPUT_STREAM_ERROR: return "ERROR";
        case UMI_OUTPUT_STREAM_SYSTEM: return "SYSTEM";
        case UMI_OUTPUT_STREAM_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}
