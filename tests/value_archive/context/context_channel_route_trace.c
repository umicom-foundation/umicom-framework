/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_route_trace.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/route_trace.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/route_trace.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextRouteTraceTransferEqual(const UmiContextRouteTrace *a, const UmiContextRouteTrace *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->trace_id, b->trace_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->source_channel_id, b->source_channel_id) == 0 &&
        strcmp(a->target_channel_id, b->target_channel_id) == 0 &&
        strcmp(a->route_id, b->route_id) == 0 &&
        a->hop == b->hop &&
        a->status == b->status &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextRouteTraceTransferTails(UmiContextRouteTrace *value)
{
    (void)value;
    {
        size_t used = strlen(value->trace_id) + 1U;
        memset(value->trace_id + used, 0xa5, sizeof(value->trace_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->source_channel_id) + 1U;
        memset(value->source_channel_id + used, 0xa5, sizeof(value->source_channel_id) - used);
    }
    {
        size_t used = strlen(value->target_channel_id) + 1U;
        memset(value->target_channel_id + used, 0xa5, sizeof(value->target_channel_id) - used);
    }
    {
        size_t used = strlen(value->route_id) + 1U;
        memset(value->route_id + used, 0xa5, sizeof(value->route_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextRouteTraceTransferMalformed(const UmiContextRouteTrace *sample)
{
    (void)sample;
    {
        UmiContextRouteTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.trace_id, 'x', sizeof(invalid.trace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated trace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextRouteTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextRouteTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_channel_id, 'x', sizeof(invalid.source_channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextRouteTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_channel_id, 'x', sizeof(invalid.target_channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextRouteTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.route_id, 'x', sizeof(invalid.route_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_route_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_route_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated route_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextRouteTraceTransferCases, UmiContextRouteTrace,
    umi_context_route_trace_archive_encode, umi_context_route_trace_archive_decode,
    UmiContextRouteTraceTransferEqual, UmiContextRouteTraceTransferTails, UmiContextRouteTraceTransferMalformed)

int main(void)
{
    UmiContextRouteTrace value;
    umi_context_route_trace_init(&value);
    value.trace_id[0] = 's';
    value.context_id[0] = 's';
    value.source_channel_id[0] = 's';
    value.target_channel_id[0] = 's';
    value.route_id[0] = 's';
    value.hop = (uint32_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_route_trace_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextRouteTraceTransferCases(&value) != 0) return 1;

    return 0;
}
