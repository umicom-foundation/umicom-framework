/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_ledger_account.c
 *
 * PURPOSE:
 *   Exercise ledger account validation and calculations.
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
#include "umicom/finance/accounting/ledger_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/accounting/ledger_account.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAccountingLedgerAccountTransferEqual(const UmiAccountingLedgerAccount *a, const UmiAccountingLedgerAccount *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->account_class == b->account_class &&
        a->normal_side == b->normal_side &&
        a->posting_allowed == b->posting_allowed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAccountingLedgerAccountTransferTails(UmiAccountingLedgerAccount *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAccountingLedgerAccountTransferMalformed(const UmiAccountingLedgerAccount *sample)
{
    (void)sample;
    {
        UmiAccountingLedgerAccount invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_accounting_ledger_account_valid(&invalid)) ||
            umi_accounting_ledger_account_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAccountingLedgerAccount invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_accounting_ledger_account_valid(&invalid)) ||
            umi_accounting_ledger_account_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAccountingLedgerAccountTransferCases, UmiAccountingLedgerAccount,
    umi_accounting_ledger_account_archive_encode, umi_accounting_ledger_account_archive_decode,
    UmiAccountingLedgerAccountTransferEqual, UmiAccountingLedgerAccountTransferTails, UmiAccountingLedgerAccountTransferMalformed)

int main(void) {
    UmiAccountingLedgerAccount v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_ledger_account_init(&v, "1000", "Cash", UMI_ACCOUNTING_ASSET, UMI_ACCOUNTING_NORMAL_DEBIT, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_accounting_ledger_account_valid(&v)) return 2;
    if (UmiAccountingLedgerAccountTransferCases(&v) != 0) return 1;

    return 0;
}
