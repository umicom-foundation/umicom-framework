/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_24_context_trace.c
 *
 * PURPOSE:
 *   Validate context trace sequence accounting, bounded fields and failure evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_trace.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_trace.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextTraceTransferEqual(const UmiContextTrace *a, const UmiContextTrace *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->trace_id, b->trace_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->correlation_id, b->correlation_id) == 0 &&
        strcmp(a->route_id, b->route_id) == 0 &&
        a->first_sequence == b->first_sequence &&
        a->last_sequence == b->last_sequence &&
        a->item_count == b->item_count &&
        a->failure_count == b->failure_count &&
        a->status == b->status &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextTraceTransferTails(UmiContextTrace *value)
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
        size_t used = strlen(value->correlation_id) + 1U;
        memset(value->correlation_id + used, 0xa5, sizeof(value->correlation_id) - used);
    }
    {
        size_t used = strlen(value->route_id) + 1U;
        memset(value->route_id + used, 0xa5, sizeof(value->route_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextTraceTransferMalformed(const UmiContextTrace *sample)
{
    (void)sample;
    {
        UmiContextTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.trace_id, 'x', sizeof(invalid.trace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated trace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.correlation_id, 'x', sizeof(invalid.correlation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated correlation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTrace invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.route_id, 'x', sizeof(invalid.route_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_trace_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_trace_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated route_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextTraceTransferCases, UmiContextTrace,
    umi_context_trace_archive_encode, umi_context_trace_archive_decode,
    UmiContextTraceTransferEqual, UmiContextTraceTransferTails, UmiContextTraceTransferMalformed)

int main(void)
{
    UmiContextTrace state;
    umi_context_trace_init(&state);
    assert(umi_context_trace_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_trace_field(&state,0U),"alpha") == 0);
    assert(umi_context_trace_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_trace_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_trace_covers_sequence(&state,10U));
    assert(umi_context_trace_covers_sequence(&state,11U));
    assert(umi_context_trace_validate(&state) == UMI_STATUS_OK);
    if (UmiContextTraceTransferCases(&state) != 0) return 1;

    return 0;
}
