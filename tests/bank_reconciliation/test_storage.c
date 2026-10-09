/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reconciliation/test_storage.c
 * PURPOSE: Check durable investigation replay, stale writers and atomic disposition rollback.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../../src/bank_operations/internal.h"

/* Use a newly reserved test database; never replace an existing profile file. */
int main(int argc, char **argv)
{
    CHECK(argc == 3); const char *name = argv[1]; Fixture f = {0};
    FILE *created = fopen(argv[2], "wx"); CHECK(created != NULL && fclose(created) == 0);
    UmiStatus status = UmiBankOperationsOpenSqlite(argv[2], &f.bank);
    if (status == UMI_STATUS_UNAVAILABLE) { CHECK(remove(argv[2]) == 0); return 77; }
    OK(status); ReconciliationSetup(&f);
    UmiBankCommand c = Resolution(&f); UmiBankReview *review = NULL; UmiBankReceipt receipt;
    OK(UmiBankOperationsReview(f.bank, &f.operator, &c, &review));
    if (strcmp(name, "restart") == 0 || strcmp(name, "reopen-restart") == 0) {
        OK(UmiBankOperationsExecuteReviewed(f.bank, &f.operator, review, &receipt));
        if (strcmp(name, "reopen-restart") == 0) { UmiBankCommand open = Reopen(&f); ApplyResolution(&f, &open); }
        UmiBankOperationsDestroy(f.bank); f.bank = NULL;
        OK(UmiBankOperationsOpenSqlite(argv[2], &f.bank));
        UmiBankReconciliation record = ReadBreak(&f);
        CHECK(!record.matched && record.externalBalance.minor_units == 90000 && record.bookedBalance.minor_units == 100000 && record.revision == 4U);
        CHECK(record.disposition == (strcmp(name, "restart") == 0 ? UMI_BANK_RECONCILIATION_RESOLVED : UMI_BANK_RECONCILIATION_REOPENED));
        CHECK(strcmp(record.evidenceId.value, "match") == 0 && strcmp(record.reviewedBy.value, "operator") == 0);
        UmiBankCounts counts; OK(UmiBankOperationsCounts(f.bank, &counts));
        CHECK(counts.durable && counts.journals == 1U && counts.reconciliations == 2U);
        /* Replaying the historical resolve receipt must not close a reopened break. */
        OK(UmiBankOperationsExecute(f.bank, &f.operator, &c, &receipt)); CHECK(receipt.idempotent && receipt.revision == 6U);
        CHECK(ReadBreak(&f).disposition == record.disposition && ReadBreak(&f).reviewedRevision == record.reviewedRevision);
    } else if (strcmp(name, "write-abort") == 0 || strcmp(name, "write-rollback") == 0) {
        const char *trigger = strcmp(name, "write-abort") == 0 ?
            "CREATE TRIGGER reconciliation_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='bank.operations.revision' BEGIN SELECT RAISE(ABORT,'review failure'); END;" :
            "CREATE TRIGGER reconciliation_fail BEFORE UPDATE ON umicom_kv WHEN NEW.key='bank.operations.revision' BEGIN SELECT RAISE(ROLLBACK,'review failure'); END;";
        OK(umi_data_server_execute(f.bank->server, trigger));
        CHECK(UmiBankOperationsExecuteReviewed(f.bank, &f.operator, review, &receipt) != UMI_STATUS_OK && receipt.revision == 0U);
/* The old fixture used cached state after a transaction-loss error. Reopen the poisoned handle and then check the same saved balances; retain the original assertion for review. */
#if 0
        CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_UNREVIEWED);
#endif
        /* A whole-transaction abort leaves the banking handle deliberately
         * poisoned. Reopen through durable replay before trusting its balances;
         * statement-only ABORT still permits an explicit successful rollback. */
        if (strcmp(name, "write-rollback") == 0) {
            CHECK(f.bank->poisoned);
            UmiBankOperationsDestroy(f.bank);
            f.bank = NULL;
            OK(UmiBankOperationsOpenSqlite(argv[2], &f.bank));
        }
        CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_UNREVIEWED);
        OK(UmiBankOperationsReload(f.bank)); CHECK(ReadBreak(&f).reviewedRevision == 0U);
        UmiBankCounts counts; OK(UmiBankOperationsCounts(f.bank, &counts)); CHECK(counts.revision == 5U && counts.journals == 1U);
        OK(umi_data_server_execute(f.bank->server, "DROP TRIGGER reconciliation_fail;"));
        OK(UmiBankOperationsExecuteReviewed(f.bank, &f.operator, review, &receipt));
        CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_RESOLVED);
    } else if (strcmp(name, "stale-writer") == 0) {
        Fixture other = {0}; OK(UmiBankOperationsOpenSqlite(argv[2], &other.bank));
        other.operator = f.operator; other.serial = 100;
        Compare(&other, "newer-observation", "account", 95000);
        CHECK(UmiBankOperationsExecuteReviewed(f.bank, &f.operator, review, &receipt) == UMI_STATUS_BUSY && receipt.revision == 0U);
        OK(UmiBankOperationsReload(f.bank)); CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_UNREVIEWED);
        c = Resolution(&f); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_BUSY);
        UmiBankOperationsDestroy(other.bank);
    } else if (strcmp(name, "impossible-replay") == 0) {
        OK(UmiBankOperationsExecuteReviewed(f.bank, &f.operator, review, &receipt));
        UmiBankAuditEvent event; OK(UmiBankOperationsAuditAt(f.bank, 5U, &event));
        Id(&event.command.ownerId, "break"); /* An unmatched comparison is not resolution evidence. */
        char encoded[BANK_RECORD_TEXT_CAPACITY]; OK(BankEncode(&event, encoded, sizeof encoded));
        OK(umi_data_server_set(f.bank->server, BANK_EVENT_KEY_PREFIX "00000000000000000006", encoded));
        CHECK(UmiBankOperationsReload(f.bank) == UMI_STATUS_PARSE_ERROR);
        CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_RESOLVED && strcmp(ReadBreak(&f).evidenceId.value, "match") == 0);
    } else return 2;
    Balance(&f, 100000, 0);
    UmiBankReviewDestroy(review); UmiBankOperationsDestroy(f.bank); CHECK(remove(argv[2]) == 0); return 0;
}
