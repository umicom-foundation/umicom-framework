/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_activity/test_capture.c
 * PURPOSE: Assert filtered postings without changing complete balances, reservations or event order.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0}; OK(UmiBankOperationsOpenMemory(&f.bank));
    ActivitySetup(&f); UmiBankActivityQuery query = ActivityQuery(); UmiBankActivity *report = NULL;
    UmiBankCounts before, after; OK(UmiBankOperationsCounts(f.bank, &before));
    UmiBankActivitySummary summary; UmiBankActivityRow row;
    if (strcmp(name, "empty-account") == 0 || strcmp(name, "closed-account") == 0) {
        UmiBankCommand c = Make(&f, UMI_BANK_ACCOUNT_OPEN, "empty-account");
        Id(&c.ownerId, "customer"); strcpy(c.name, "Unfunded account"); c.amount = Cash(0); Send(&f, &f.maker, &c);
        if (strcmp(name, "closed-account") == 0) {
            c = Make(&f, UMI_BANK_ACCOUNT_SET_STATE, "empty-account"); c.state = UMI_BANK_RECORD_CLOSED; Send(&f, &f.maker, &c);
        }
        Id(&query.accountId, "empty-account"); report = ActivityCapture(&f, query); OK(UmiBankActivityReadSummary(report, &summary));
        CHECK(summary.totalPostings == 0 && summary.count == 0 && summary.balance.booked.minor_units == 0 && summary.netMinor == 0);
        UmiBankActivityDestroy(report); UmiBankOperationsDestroy(f.bank); return 0;
    }
    if (strcmp(name, "date-range") == 0) { query.fromDate = (UmiFinancialDate){2026,10,1}; query.toDate = (UmiFinancialDate){2026,10,2}; }
    else if (strcmp(name, "backdated") == 0) query.fromDate = query.toDate = (UmiFinancialDate){2026,9,29};
    else if (strcmp(name, "debits") == 0) query.direction = UMI_BANK_ACTIVITY_DEBITS;
    else if (strcmp(name, "credits") == 0) query.direction = UMI_BANK_ACTIVITY_CREDITS;
    else if (strcmp(name, "reference") == 0) strcpy(query.reference, "charg");
    else if (strcmp(name, "journal") == 0) strcpy(query.reference, "journal-7");
    else if (strcmp(name, "case-sensitive") == 0) strcpy(query.reference, "CHARGE");
    else if (strcmp(name, "empty") == 0) query.fromDate = (UmiFinancialDate){2027,1,1};
    else if (strcmp(name, "unknown") == 0) {
        Id(&query.accountId, "missing"); CHECK(UmiBankActivityCapture(f.bank, &query, &report) == UMI_STATUS_NOT_FOUND && report == NULL);
        Id(&query.accountId, "account");
    } else if (strcmp(name, "overflow") == 0) {
        /* A legitimate credit/debit cycle can overflow turnover totals even
         * though each intermediate account balance remains valid. */
        UmiBankCommand c = Make(&f, UMI_BANK_TEST_CREDIT, "large");
        Id(&c.sourceAccountId, "account"); c.amount = Cash(INT64_MAX - 100500); Send(&f, &f.operator, &c);
        CHECK(UmiBankActivityCapture(f.bank, &query, &report) == UMI_STATUS_CAPACITY_EXCEEDED && report == NULL);
        query.direction = UMI_BANK_ACTIVITY_DEBITS; report = ActivityCapture(&f, query);
        OK(UmiBankActivityReadSummary(report, &summary)); CHECK(summary.debitMinor == 2500 && summary.balance.booked.minor_units == INT64_MAX);
        UmiBankActivityDestroy(report); UmiBankOperationsDestroy(f.bank); return 0;
    } else if (strcmp(name, "all") != 0 && strcmp(name, "ownership") != 0 && strcmp(name, "bounds") != 0) return 2;
    report = ActivityCapture(&f, query); OK(UmiBankActivityReadSummary(report, &summary));
    CHECK(summary.totalPostings == 4 && summary.balance.booked.minor_units == 100500 && summary.balance.reserved.minor_units == 1000 && summary.balance.available.minor_units == 99500);
    CHECK(summary.revision == before.revision && !summary.durable);
    if (strcmp(name, "date-range") == 0) CHECK(summary.count == 2 && summary.creditMinor == 500 && summary.debitMinor == 2500 && summary.netMinor == -2000);
    else if (strcmp(name, "backdated") == 0) {
        CHECK(summary.count == 1 && summary.creditMinor == 2500); OK(UmiBankActivityRowAt(report, 0, &row));
        CHECK(row.reversal && row.action == UMI_BANK_CHARGE_REVERSE && row.posting.balanceMinor == 100500);
    } else if (strcmp(name, "debits") == 0 || strcmp(name, "journal") == 0) {
        CHECK(summary.count == 1 && summary.debitMinor == 2500 && summary.netMinor == -2500);
        OK(UmiBankActivityRowAt(report, 0, &row)); CHECK(row.posting.balanceMinor == 98000 && !row.reversal);
    } else if (strcmp(name, "credits") == 0) CHECK(summary.count == 3 && summary.creditMinor == 103000 && summary.debitMinor == 0);
    else if (strcmp(name, "reference") == 0) CHECK(summary.count == 2 && summary.creditMinor == 2500 && summary.debitMinor == 2500 && summary.netMinor == 0);
    else if (strcmp(name, "empty") == 0 || strcmp(name, "case-sensitive") == 0) CHECK(summary.count == 0 && summary.netMinor == 0);
    else {
        CHECK(summary.count == 4 && summary.creditMinor == 103000 && summary.debitMinor == 2500 && summary.netMinor == 100500);
        OK(UmiBankActivityRowAt(report, 2, &row)); CHECK(row.posting.revision == 7 && row.posting.businessDate.month == 10 && strcmp(row.actorId.value, "operator") == 0);
        OK(UmiBankActivityRowAt(report, 3, &row)); CHECK(row.posting.revision == 8 && row.posting.businessDate.month == 9);
    }
    OK(UmiBankOperationsCounts(f.bank, &after)); CHECK(after.revision == before.revision && after.journals == before.journals && after.holds == before.holds);
    if (strcmp(name, "bounds") == 0) {
        memset(&row, 0x5a, sizeof row); UmiBankActivityRow saved; memcpy(&saved, &row, sizeof saved);
        CHECK(UmiBankActivityRowAt(report, summary.count, &row) == UMI_STATUS_NOT_FOUND && memcmp(&row, &saved, sizeof row) == 0);
        CHECK(UmiBankActivityReadSummary(NULL, &summary) == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (strcmp(name, "ownership") == 0) {
        query.reference[0] = 'x'; UmiBankOperationsDestroy(f.bank); f.bank = NULL;
        OK(UmiBankActivityReadSummary(report, &summary)); CHECK(summary.query.reference[0] == '\0' && summary.count == 4);
        OK(UmiBankActivityRowAt(report, 0, &row)); CHECK(strcmp(row.posting.referenceId.value, "funding") == 0);
    }
    UmiBankActivityDestroy(report); UmiBankOperationsDestroy(f.bank); return 0;
}
