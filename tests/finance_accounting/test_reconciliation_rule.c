/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_reconciliation_rule.c
 *
 * PURPOSE:
 *   Exercise reconciliation rule validation and calculations.
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
#include "umicom/finance/accounting/reconciliation_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/accounting/reconciliation_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAccountingReconciliationRuleTransferEqual(const UmiAccountingReconciliationRule *a, const UmiAccountingReconciliationRule *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        a->tolerance_minor == b->tolerance_minor &&
        a->auto_match == b->auto_match;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAccountingReconciliationRuleTransferTails(UmiAccountingReconciliationRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAccountingReconciliationRuleTransferMalformed(const UmiAccountingReconciliationRule *sample)
{
    (void)sample;
    {
        UmiAccountingReconciliationRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_accounting_reconciliation_rule_valid(&invalid)) ||
            umi_accounting_reconciliation_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAccountingReconciliationRuleTransferCases, UmiAccountingReconciliationRule,
    umi_accounting_reconciliation_rule_archive_encode, umi_accounting_reconciliation_rule_archive_decode,
    UmiAccountingReconciliationRuleTransferEqual, UmiAccountingReconciliationRuleTransferTails, UmiAccountingReconciliationRuleTransferMalformed)

int main(void) {
    UmiAccountingReconciliationRule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_reconciliation_rule_init(&v, "recon-rule", 5, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_accounting_reconciliation_rule_valid(&v)) return 2;
    if (UmiAccountingReconciliationRuleTransferCases(&v) != 0) return 1;

    return 0;
}
