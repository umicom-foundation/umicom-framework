/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_posting_rule.c
 *
 * PURPOSE:
 *   Exercise posting rule validation and calculations.
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
#include "umicom/finance/accounting/posting_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/accounting/posting_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAccountingPostingRuleTransferEqual(const UmiAccountingPostingRule *a, const UmiAccountingPostingRule *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->event_type, b->event_type) == 0 &&
        strcmp(a->debit_account_id.value, b->debit_account_id.value) == 0 &&
        strcmp(a->credit_account_id.value, b->credit_account_id.value) == 0 &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAccountingPostingRuleTransferTails(UmiAccountingPostingRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->event_type) + 1U;
        memset(value->event_type + used, 0xa5, sizeof(value->event_type) - used);
    }
    {
        size_t used = strlen(value->debit_account_id.value) + 1U;
        memset(value->debit_account_id.value + used, 0xa5, sizeof(value->debit_account_id.value) - used);
    }
    {
        size_t used = strlen(value->credit_account_id.value) + 1U;
        memset(value->credit_account_id.value + used, 0xa5, sizeof(value->credit_account_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAccountingPostingRuleTransferMalformed(const UmiAccountingPostingRule *sample)
{
    (void)sample;
    {
        UmiAccountingPostingRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_accounting_posting_rule_valid(&invalid)) ||
            umi_accounting_posting_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountingPostingRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.event_type, 'x', sizeof(invalid.event_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_accounting_posting_rule_valid(&invalid)) ||
            umi_accounting_posting_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated event_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountingPostingRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.debit_account_id.value, 'x', sizeof(invalid.debit_account_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_accounting_posting_rule_valid(&invalid)) ||
            umi_accounting_posting_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated debit_account_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountingPostingRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.credit_account_id.value, 'x', sizeof(invalid.credit_account_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_accounting_posting_rule_valid(&invalid)) ||
            umi_accounting_posting_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated credit_account_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAccountingPostingRuleTransferCases, UmiAccountingPostingRule,
    umi_accounting_posting_rule_archive_encode, umi_accounting_posting_rule_archive_decode,
    UmiAccountingPostingRuleTransferEqual, UmiAccountingPostingRuleTransferTails, UmiAccountingPostingRuleTransferMalformed)

int main(void) {
    UmiAccountingPostingRule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_posting_rule_init(&v, "rule-1", "CASH_RECEIPT", "1000", "4000", true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_accounting_posting_rule_valid(&v)) return 2;
    if (UmiAccountingPostingRuleTransferCases(&v) != 0) return 1;

    return 0;
}
