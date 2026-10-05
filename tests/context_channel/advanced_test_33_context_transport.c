/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_33_context_transport.c
 *
 * PURPOSE:
 *   Validate context transport sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_transport.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_transport.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextTransportTransferEqual(const UmiContextTransport *a, const UmiContextTransport *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->transport_id, b->transport_id) == 0 &&
        strcmp(a->endpoint_id, b->endpoint_id) == 0 &&
        strcmp(a->protocol, b->protocol) == 0 &&
        strcmp(a->address, b->address) == 0 &&
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
static void UmiContextTransportTransferTails(UmiContextTransport *value)
{
    (void)value;
    {
        size_t used = strlen(value->transport_id) + 1U;
        memset(value->transport_id + used, 0xa5, sizeof(value->transport_id) - used);
    }
    {
        size_t used = strlen(value->endpoint_id) + 1U;
        memset(value->endpoint_id + used, 0xa5, sizeof(value->endpoint_id) - used);
    }
    {
        size_t used = strlen(value->protocol) + 1U;
        memset(value->protocol + used, 0xa5, sizeof(value->protocol) - used);
    }
    {
        size_t used = strlen(value->address) + 1U;
        memset(value->address + used, 0xa5, sizeof(value->address) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextTransportTransferMalformed(const UmiContextTransport *sample)
{
    (void)sample;
    {
        UmiContextTransport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.transport_id, 'x', sizeof(invalid.transport_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transport_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transport_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated transport_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.endpoint_id, 'x', sizeof(invalid.endpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transport_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transport_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated endpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.protocol, 'x', sizeof(invalid.protocol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transport_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transport_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated protocol was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.address, 'x', sizeof(invalid.address));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transport_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transport_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated address was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextTransportTransferCases, UmiContextTransport,
    umi_context_transport_archive_encode, umi_context_transport_archive_decode,
    UmiContextTransportTransferEqual, UmiContextTransportTransferTails, UmiContextTransportTransferMalformed)

int main(void)
{
    UmiContextTransport state;
    umi_context_transport_init(&state);
    assert(umi_context_transport_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_transport_field(&state,0U),"alpha") == 0);
    assert(umi_context_transport_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_transport_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_transport_covers_sequence(&state,10U));
    assert(umi_context_transport_covers_sequence(&state,11U));
    assert(umi_context_transport_validate(&state) == UMI_STATUS_OK);
    if (UmiContextTransportTransferCases(&state) != 0) return 1;

    return 0;
}
