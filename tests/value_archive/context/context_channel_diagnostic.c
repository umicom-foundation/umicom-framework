/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_diagnostic.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/diagnostic.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/diagnostic.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextDiagnosticTransferEqual(const UmiContextDiagnostic *a, const UmiContextDiagnostic *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->diagnostic_id, b->diagnostic_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->message, b->message) == 0 &&
        a->severity == b->severity &&
        a->status == b->status &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextDiagnosticTransferTails(UmiContextDiagnostic *value)
{
    (void)value;
    {
        size_t used = strlen(value->diagnostic_id) + 1U;
        memset(value->diagnostic_id + used, 0xa5, sizeof(value->diagnostic_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextDiagnosticTransferMalformed(const UmiContextDiagnostic *sample)
{
    (void)sample;
    {
        UmiContextDiagnostic invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.diagnostic_id, 'x', sizeof(invalid.diagnostic_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_diagnostic_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_diagnostic_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated diagnostic_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextDiagnostic invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_diagnostic_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_diagnostic_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextDiagnostic invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_diagnostic_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_diagnostic_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextDiagnostic invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_diagnostic_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_diagnostic_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextDiagnosticTransferCases, UmiContextDiagnostic,
    umi_context_diagnostic_archive_encode, umi_context_diagnostic_archive_decode,
    UmiContextDiagnosticTransferEqual, UmiContextDiagnosticTransferTails, UmiContextDiagnosticTransferMalformed)

int main(void)
{
    UmiContextDiagnostic value;
    umi_context_diagnostic_init(&value);
    value.diagnostic_id[0] = 's';
    value.channel_id[0] = 's';
    value.context_id[0] = 's';
    value.message[0] = 's';
    value.severity = (uint32_t)17U;
    value.timestamp_ms = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_diagnostic_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextDiagnosticTransferCases(&value) != 0) return 1;

    return 0;
}
