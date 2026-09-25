/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance_operations/reporting.c
 *
 * PURPOSE:
 *   Build dated trial balances from the existing canonical posted journals without mixing currencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <limits.h>
#include <string.h>

UmiStatus FinanceTrialBalance(const FinanceState *state, UmiFinancialId periodId,
    UmiCurrency currency, uint8_t scale, UmiFinanceTrialBalance *out)
{
    size_t periodIndex = FinancePeriodIndex(state, periodId);
    UmiFinanceTrialBalance report;
    const UmiFinanceOperationPeriod *period;
    UmiStatus status = UMI_STATUS_OK;
    if (out == NULL || !FinanceCurrencyValid(currency) || scale > 9U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (periodIndex == FINANCE_INDEX_NONE) return UMI_STATUS_NOT_FOUND;
    memset(&report, 0, sizeof(report));
    report.periodId = periodId;
    report.currency = currency;
    report.scale = scale;
    period = &state->periods[periodIndex];
    for (size_t accountIndex = 0U; accountIndex < state->counts.accounts; ++accountIndex) {
        const UmiFinanceOperationAccount *account = &state->accounts[accountIndex];
        UmiFinanceTrialBalanceLine *line;
        if (!umi_accounting_currency_equal(account->currency, currency)) continue;
        if (account->scale != scale) return UMI_STATUS_INVALID_ARGUMENT;
        line = &report.lines[report.lineCount++];
        line->accountId = account->id;
        line->accountClass = account->accountClass;
        for (size_t journalIndex = 0U; journalIndex < state->counts.journals; ++journalIndex) {
            const UmiAccountingJournalEntry *journal = &state->journals[journalIndex].entry;
            bool opening;
            /* A reversed original still happened. Its compensating journal is
             * a second posting on its own date; neither entry is discarded. */
            if (journal->status != UMI_ACCOUNTING_JOURNAL_POSTED &&
                journal->status != UMI_ACCOUNTING_JOURNAL_REVERSED) continue;
            if (umi_financial_date_compare(journal->accounting_date, period->endDate) > 0) continue;
            opening = umi_financial_date_compare(journal->accounting_date, period->startDate) < 0;
            for (size_t item = 0U; item < journal->line_count; ++item) {
                const UmiAccountingJournalLine *posting = &journal->lines[item];
                if (!FinanceIdEqual(posting->account_id, account->id)) continue;
                if (opening) {
                    status = FinanceAdd(line->openingDebitNet,
                        posting->debit_minor - posting->credit_minor, &line->openingDebitNet);
                } else {
                    status = FinanceAdd(line->periodDebitMinor, posting->debit_minor, &line->periodDebitMinor);
                    if (status == UMI_STATUS_OK) status = FinanceAdd(line->periodCreditMinor,
                        posting->credit_minor, &line->periodCreditMinor);
                }
                if (status != UMI_STATUS_OK) return status;
            }
        }
        status = FinanceAdd(line->openingDebitNet,
            line->periodDebitMinor - line->periodCreditMinor, &line->closingDebitNet);
        if (status == UMI_STATUS_OK) status = FinanceAdd(report.periodDebitMinor,
            line->periodDebitMinor, &report.periodDebitMinor);
        if (status == UMI_STATUS_OK) status = FinanceAdd(report.periodCreditMinor,
            line->periodCreditMinor, &report.periodCreditMinor);
        if (status != UMI_STATUS_OK) return status;
        if (line->closingDebitNet == INT64_MIN) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (line->closingDebitNet >= 0) status = FinanceAdd(report.closingDebitMinor,
            line->closingDebitNet, &report.closingDebitMinor);
        else status = FinanceAdd(report.closingCreditMinor, -line->closingDebitNet, &report.closingCreditMinor);
        if (status != UMI_STATUS_OK) return status;
    }
    if (report.lineCount == 0U) return UMI_STATUS_NOT_FOUND;
    report.balanced = report.periodDebitMinor == report.periodCreditMinor &&
        report.closingDebitMinor == report.closingCreditMinor;
    *out = report;
    return UMI_STATUS_OK;
}
