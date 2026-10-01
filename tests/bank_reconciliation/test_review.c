/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reconciliation/test_review.c
 * PURPOSE: Check immutable investigation evidence, form binding and canonical history guards.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

/* An owned review remains readable after its service disappears. */
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0};
    OK(UmiBankOperationsOpenMemory(&f.bank)); ReconciliationSetup(&f);
    UmiBankCommand c = Resolution(&f); UmiBankReview *review = NULL; UmiBankReceipt receipt;
    OK(UmiBankOperationsReview(f.bank, &f.operator, &c, &review));
    UmiBankReviewSnapshot *snapshot = calloc(1U, sizeof *snapshot); CHECK(snapshot != NULL);
    OK(UmiBankReviewSnapshotRead(review, snapshot));
    CHECK(snapshot->hasReconciliation && snapshot->reconciliationExistedBefore && snapshot->hasReconciliationEvidence);
    CHECK(!snapshot->hasJournal && snapshot->accountCount == 0U);
    CHECK(snapshot->reconciliationBefore.disposition == UMI_BANK_RECONCILIATION_UNREVIEWED &&
        snapshot->reconciliation.disposition == UMI_BANK_RECONCILIATION_RESOLVED &&
        snapshot->reconciliationEvidence.matched && snapshot->reconciliationEvidence.revision == 5U);
    if (strcmp(name, "describe") == 0 || strcmp(name, "ownership") == 0) {
        if (strcmp(name, "ownership") == 0) { UmiBankOperationsDestroy(f.bank); f.bank = NULL; }
        size_t required = 0U;
        CHECK(UmiBankReviewDescribe(review, NULL, 0U, &required) == UMI_STATUS_CAPACITY_EXCEEDED && required > 0U);
        char *text = malloc(required); CHECK(text != NULL); OK(UmiBankReviewDescribe(review, text, required, NULL));
        CHECK(strstr(text, "Original external: GBP 900.00") && strstr(text, "original booked: GBP 1000.00"));
        CHECK(strstr(text, "Open break -> Resolved break") && strstr(text, "Linked comparison match") && strstr(text, c.name));
        free(text);
    } else if (strcmp(name, "edit") == 0) {
        bool matches = true; strcpy(c.name, "New reason");
        OK(UmiBankReviewMatches(review, &f.operator, &c, &matches)); CHECK(!matches);
        UmiBankActor changed = f.operator; changed.capabilities = UMI_BANK_CAP_CUSTOMERS;
        CHECK(UmiBankOperationsExecuteReviewed(f.bank, &changed, review, &receipt) == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(name, "stale") == 0) {
        Compare(&f, "new-match", "account", 100000);
        CHECK(UmiBankOperationsExecuteReviewed(f.bank, &f.operator, review, &receipt) == UMI_STATUS_BUSY && receipt.revision == 0U);
        CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_UNREVIEWED);
    } else if (strcmp(name, "history") == 0 || strcmp(name, "equivalent") == 0) {
        Fixture other = {0}; OK(UmiBankOperationsOpenMemory(&other.bank)); Setup(&other);
        Compare(&other, "break", "account", strcmp(name, "history") == 0 ? 95000 : 90000);
        Compare(&other, "match", "account", 100000);
        UmiStatus status = UmiBankOperationsExecuteReviewed(other.bank, &other.operator, review, &receipt);
        CHECK(status == (strcmp(name, "history") == 0 ? UMI_STATUS_BUSY : UMI_STATUS_OK));
        CHECK(ReadBreak(&f).disposition == UMI_BANK_RECONCILIATION_UNREVIEWED);
        UmiBankOperationsDestroy(other.bank);
    } else return 2;
    free(snapshot); UmiBankReviewDestroy(review); UmiBankOperationsDestroy(f.bank); return 0;
}
