/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_06_context_import.c
 *
 * PURPOSE:
 *   Validate context import sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_import.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_import.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextImportTransferEqual(const UmiContextImport *a, const UmiContextImport *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->import_id, b->import_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->source_name, b->source_name) == 0 &&
        strcmp(a->target_channel, b->target_channel) == 0 &&
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
static void UmiContextImportTransferTails(UmiContextImport *value)
{
    (void)value;
    {
        size_t used = strlen(value->import_id) + 1U;
        memset(value->import_id + used, 0xa5, sizeof(value->import_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->source_name) + 1U;
        memset(value->source_name + used, 0xa5, sizeof(value->source_name) - used);
    }
    {
        size_t used = strlen(value->target_channel) + 1U;
        memset(value->target_channel + used, 0xa5, sizeof(value->target_channel) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextImportTransferMalformed(const UmiContextImport *sample)
{
    (void)sample;
    {
        UmiContextImport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.import_id, 'x', sizeof(invalid.import_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_import_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_import_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated import_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextImport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_import_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_import_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextImport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_name, 'x', sizeof(invalid.source_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_import_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_import_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextImport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_channel, 'x', sizeof(invalid.target_channel));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_import_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_import_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_channel was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextImportTransferCases, UmiContextImport,
    umi_context_import_archive_encode, umi_context_import_archive_decode,
    UmiContextImportTransferEqual, UmiContextImportTransferTails, UmiContextImportTransferMalformed)

int main(void)
{
    UmiContextImport state;
    umi_context_import_init(&state);
    assert(umi_context_import_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_import_field(&state,0U),"alpha") == 0);
    assert(umi_context_import_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_import_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_import_covers_sequence(&state,10U));
    assert(umi_context_import_covers_sequence(&state,11U));
    assert(umi_context_import_validate(&state) == UMI_STATUS_OK);
    if (UmiContextImportTransferCases(&state) != 0) return 1;

    return 0;
}
