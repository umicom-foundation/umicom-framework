/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_15_context_group_service.c
 *
 * PURPOSE:
 *   Validate context group service sequence accounting, bounded fields and failure evidence.
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
#include "umicom/context_channel/context_group_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_group_service.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextGroupServiceTransferEqual(const UmiContextGroupService *a, const UmiContextGroupService *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->operation_id, b->operation_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->member_id, b->member_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
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
static void UmiContextGroupServiceTransferTails(UmiContextGroupService *value)
{
    (void)value;
    {
        size_t used = strlen(value->operation_id) + 1U;
        memset(value->operation_id + used, 0xa5, sizeof(value->operation_id) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->member_id) + 1U;
        memset(value->member_id + used, 0xa5, sizeof(value->member_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextGroupServiceTransferMalformed(const UmiContextGroupService *sample)
{
    (void)sample;
    {
        UmiContextGroupService invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation_id, 'x', sizeof(invalid.operation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_group_service_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_group_service_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextGroupService invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_group_service_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_group_service_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextGroupService invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.member_id, 'x', sizeof(invalid.member_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_group_service_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_group_service_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated member_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextGroupService invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_group_service_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_group_service_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextGroupServiceTransferCases, UmiContextGroupService,
    umi_context_group_service_archive_encode, umi_context_group_service_archive_decode,
    UmiContextGroupServiceTransferEqual, UmiContextGroupServiceTransferTails, UmiContextGroupServiceTransferMalformed)

int main(void)
{
    UmiContextGroupService state;
    umi_context_group_service_init(&state);
    assert(umi_context_group_service_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_group_service_field(&state,0U),"alpha") == 0);
    assert(umi_context_group_service_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_group_service_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_group_service_covers_sequence(&state,10U));
    assert(umi_context_group_service_covers_sequence(&state,11U));
    assert(umi_context_group_service_validate(&state) == UMI_STATUS_OK);
    if (UmiContextGroupServiceTransferCases(&state) != 0) return 1;

    return 0;
}
