/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_20_context_validator.c
 *
 * PURPOSE:
 *   Validate context validator sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_validator.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_validator.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextValidatorTransferEqual(const UmiContextValidator *a, const UmiContextValidator *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->validation_id, b->validation_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->message, b->message) == 0 &&
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
static void UmiContextValidatorTransferTails(UmiContextValidator *value)
{
    (void)value;
    {
        size_t used = strlen(value->validation_id) + 1U;
        memset(value->validation_id + used, 0xa5, sizeof(value->validation_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextValidatorTransferMalformed(const UmiContextValidator *sample)
{
    (void)sample;
    {
        UmiContextValidator invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.validation_id, 'x', sizeof(invalid.validation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_validator_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_validator_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated validation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextValidator invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_validator_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_validator_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextValidator invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_validator_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_validator_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextValidator invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_validator_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_validator_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextValidatorTransferCases, UmiContextValidator,
    umi_context_validator_archive_encode, umi_context_validator_archive_decode,
    UmiContextValidatorTransferEqual, UmiContextValidatorTransferTails, UmiContextValidatorTransferMalformed)

int main(void)
{
    UmiContextValidator state;
    umi_context_validator_init(&state);
    assert(umi_context_validator_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_validator_field(&state,0U),"alpha") == 0);
    assert(umi_context_validator_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_validator_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_validator_covers_sequence(&state,10U));
    assert(umi_context_validator_covers_sequence(&state,11U));
    assert(umi_context_validator_validate(&state) == UMI_STATUS_OK);
    if (UmiContextValidatorTransferCases(&state) != 0) return 1;

    return 0;
}
