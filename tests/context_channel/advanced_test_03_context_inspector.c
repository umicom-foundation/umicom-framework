/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_03_context_inspector.c
 *
 * PURPOSE:
 *   Validate context inspector sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_inspector.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_inspector.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextInspectorTransferEqual(const UmiContextInspector *a, const UmiContextInspector *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->source_application, b->source_application) == 0 &&
        strcmp(a->source_panel, b->source_panel) == 0 &&
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
static void UmiContextInspectorTransferTails(UmiContextInspector *value)
{
    (void)value;
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->source_application) + 1U;
        memset(value->source_application + used, 0xa5, sizeof(value->source_application) - used);
    }
    {
        size_t used = strlen(value->source_panel) + 1U;
        memset(value->source_panel + used, 0xa5, sizeof(value->source_panel) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextInspectorTransferMalformed(const UmiContextInspector *sample)
{
    (void)sample;
    {
        UmiContextInspector invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_inspector_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_inspector_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextInspector invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_inspector_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_inspector_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextInspector invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_application, 'x', sizeof(invalid.source_application));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_inspector_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_inspector_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_application was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextInspector invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_panel, 'x', sizeof(invalid.source_panel));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_inspector_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_inspector_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_panel was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextInspectorTransferCases, UmiContextInspector,
    umi_context_inspector_archive_encode, umi_context_inspector_archive_decode,
    UmiContextInspectorTransferEqual, UmiContextInspectorTransferTails, UmiContextInspectorTransferMalformed)

int main(void)
{
    UmiContextInspector state;
    umi_context_inspector_init(&state);
    assert(umi_context_inspector_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_inspector_field(&state,0U),"alpha") == 0);
    assert(umi_context_inspector_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_inspector_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_inspector_covers_sequence(&state,10U));
    assert(umi_context_inspector_covers_sequence(&state,11U));
    assert(umi_context_inspector_validate(&state) == UMI_STATUS_OK);
    if (UmiContextInspectorTransferCases(&state) != 0) return 1;

    return 0;
}
