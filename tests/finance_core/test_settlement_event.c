/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_settlement_event.c
 *
 * PURPOSE:
 *   Exercise the settlement event financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <string.h>
#include "umicom/finance/core/settlement_event.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/settlement_event.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiSettlementEventTransferEqual(const UmiSettlementEvent *a, const UmiSettlementEvent *b)
{
    return strcmp(a->event_id.value, b->event_id.value) == 0 &&
        strcmp(a->parent_id.value, b->parent_id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->effective_date.year == b->effective_date.year &&
        a->effective_date.month == b->effective_date.month &&
        a->effective_date.day == b->effective_date.day &&
        a->state == b->state &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiSettlementEventTransferTails(UmiSettlementEvent *value)
{
    (void)value;
    {
        size_t used = strlen(value->event_id.value) + 1U;
        memset(value->event_id.value + used, 0xa5, sizeof(value->event_id.value) - used);
    }
    {
        size_t used = strlen(value->parent_id.value) + 1U;
        memset(value->parent_id.value + used, 0xa5, sizeof(value->parent_id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiSettlementEventTransferMalformed(const UmiSettlementEvent *sample)
{
    (void)sample;
    {
        UmiSettlementEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.event_id.value, 'x', sizeof(invalid.event_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_settlement_event_is_valid(&invalid)) ||
            umi_settlement_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated event_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSettlementEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_id.value, 'x', sizeof(invalid.parent_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_settlement_event_is_valid(&invalid)) ||
            umi_settlement_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiSettlementEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_settlement_event_is_valid(&invalid)) ||
            umi_settlement_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiSettlementEventTransferCases, UmiSettlementEvent,
    umi_settlement_event_archive_encode, umi_settlement_event_archive_decode,
    UmiSettlementEventTransferEqual, UmiSettlementEventTransferTails, UmiSettlementEventTransferMalformed)

int main(void)
{
    UmiSettlementEvent x; CHECK(umi_settlement_event_init(&x,"ID","Name","PARENT",(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_settlement_event_is_valid(&x));
    if (UmiSettlementEventTransferCases(&x) != 0) return 1;

    return 0;
}
