/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_01_context_search.c
 *
 * PURPOSE:
 *   Validate context search sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_search.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_search.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextSearchTransferEqual(const UmiContextSearch *a, const UmiContextSearch *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->query, b->query) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
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
static void UmiContextSearchTransferTails(UmiContextSearch *value)
{
    (void)value;
    {
        size_t used = strlen(value->query) + 1U;
        memset(value->query + used, 0xa5, sizeof(value->query) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextSearchTransferMalformed(const UmiContextSearch *sample)
{
    (void)sample;
    {
        UmiContextSearch invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.query, 'x', sizeof(invalid.query));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_search_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_search_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated query was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSearch invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_search_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_search_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSearch invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_search_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_search_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSearch invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_search_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_search_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextSearchTransferCases, UmiContextSearch,
    umi_context_search_archive_encode, umi_context_search_archive_decode,
    UmiContextSearchTransferEqual, UmiContextSearchTransferTails, UmiContextSearchTransferMalformed)

int main(void)
{
    UmiContextSearch state;
    umi_context_search_init(&state);
    assert(umi_context_search_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_search_field(&state,0U),"alpha") == 0);
    assert(umi_context_search_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_search_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_search_covers_sequence(&state,10U));
    assert(umi_context_search_covers_sequence(&state,11U));
    assert(umi_context_search_validate(&state) == UMI_STATUS_OK);
    if (UmiContextSearchTransferCases(&state) != 0) return 1;

    return 0;
}
