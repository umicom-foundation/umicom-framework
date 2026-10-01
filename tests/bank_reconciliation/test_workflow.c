/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reconciliation/test_workflow.c
 * PURPOSE: Verify retained comparisons, evidence freshness, permissions and non-monetary resolution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

/* Exercise each lifecycle guard through both review and direct execution. */
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; Fixture f = {0};
    OK(UmiBankOperationsOpenMemory(&f.bank)); ReconciliationSetup(&f);
    UmiBankCommand c = Resolution(&f); UmiBankReconciliation record;
    if (strcmp(name, "resolve") == 0 || strcmp(name, "idempotency") == 0 || strcmp(name, "reopen") == 0) {
        ApplyResolution(&f, &c); record = ReadBreak(&f);
        CHECK(!record.matched && record.revision == 4U && record.externalBalance.minor_units == 90000 && record.bookedBalance.minor_units == 100000);
        CHECK(record.disposition == UMI_BANK_RECONCILIATION_RESOLVED && record.reviewedRevision == 6U);
        CHECK(strcmp(record.evidenceId.value, "match") == 0 && strcmp(record.reviewedBy.value, "operator") == 0);
        CHECK(strcmp(record.reviewReason, c.name) == 0);
        if (strcmp(name, "idempotency") == 0) {
            UmiBankReceipt receipt; OK(UmiBankOperationsExecute(f.bank, &f.operator, &c, &receipt));
            CHECK(receipt.idempotent && receipt.revision == 6U);
            strcpy(c.name, "Changed explanation"); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_ALREADY_EXISTS);
        } else if (strcmp(name, "reopen") == 0) {
            c = Reopen(&f); ApplyResolution(&f, &c); record = ReadBreak(&f);
            CHECK(record.disposition == UMI_BANK_RECONCILIATION_REOPENED && record.reviewedRevision == 7U);
            CHECK(strcmp(record.evidenceId.value, "match") == 0 && strcmp(record.reviewReason, c.name) == 0);
            c = Resolution(&f); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_STATE);
            Compare(&f, "fresh", "account", 100000); c = Resolution(&f); Id(&c.ownerId, "fresh");
            ApplyResolution(&f, &c); record = ReadBreak(&f);
            CHECK(record.disposition == UMI_BANK_RECONCILIATION_RESOLVED && record.reviewedRevision == 9U);
            UmiBankAuditEvent event; OK(UmiBankOperationsAuditAt(f.bank, 5U, &event));
            CHECK(event.command.action == UMI_BANK_RECONCILIATION_RESOLVE && strcmp(event.command.ownerId.value, "match") == 0);
            OK(UmiBankOperationsAuditAt(f.bank, 6U, &event)); CHECK(event.command.action == UMI_BANK_RECONCILIATION_REOPEN);
        }
    } else if (strcmp(name, "authority") == 0) {
        ReconciliationFail(&f, &f.maker, c, UMI_STATUS_PERMISSION_DENIED);
        ReconciliationFail(&f, &f.checker, c, UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(name, "fields") == 0) {
        strcpy(c.name, "   "); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_ARGUMENT);
        c = Resolution(&f); c.ownerId.value[0] = '\0'; ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_ARGUMENT);
        c = Resolution(&f); Id(&c.sourceAccountId, "account"); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_ARGUMENT);
        c = Resolution(&f); c.amount = Cash(1); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "missing") == 0) {
        Id(&c.id, "missing"); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_NOT_FOUND);
        c = Resolution(&f); Id(&c.ownerId, "missing"); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name, "wrong-account") == 0 || strcmp(name, "unrelated-posting") == 0 || strcmp(name, "closed") == 0) {
        UmiBankCommand other = Make(&f, UMI_BANK_ACCOUNT_OPEN, "other"); Id(&other.ownerId, "customer");
        strcpy(other.name, "Other account"); other.amount = Cash(0); Send(&f, &f.maker, &other);
        if (strcmp(name, "wrong-account") == 0) {
            Compare(&f, "other-match", "other", 0); c = Resolution(&f); Id(&c.ownerId, "other-match");
            ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_STATE);
        } else if (strcmp(name, "closed") == 0) {
            Compare(&f, "closed-break", "other", 1); Compare(&f, "closed-match", "other", 0);
            other = Make(&f, UMI_BANK_ACCOUNT_SET_STATE, "other"); other.state = UMI_BANK_RECORD_CLOSED; Send(&f, &f.maker, &other);
            c = Resolution(&f); Id(&c.id, "closed-break"); Id(&c.ownerId, "closed-match"); ApplyResolution(&f, &c);
            OK(UmiBankOperationsFindReconciliation(f.bank, "closed-break", &record)); CHECK(record.disposition == UMI_BANK_RECONCILIATION_RESOLVED);
        } else {
            other = Make(&f, UMI_BANK_TEST_CREDIT, "other-funding"); Id(&other.sourceAccountId, "other"); other.amount = Cash(100);
            Send(&f, &f.operator, &other); c = Resolution(&f); ApplyResolution(&f, &c);
        }
    } else if (strcmp(name, "unmatched-evidence") == 0 || strcmp(name, "superseded-evidence") == 0) {
        Compare(&f, "new-break", "account", 95000); c = Resolution(&f);
        if (strcmp(name, "unmatched-evidence") == 0) Id(&c.ownerId, "new-break");
        ReconciliationFail(&f, &f.operator, c, strcmp(name, "unmatched-evidence") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_BUSY);
    } else if (strcmp(name, "older-evidence") == 0) {
        Compare(&f, "new-break", "account", 95000); c = Resolution(&f); Id(&c.id, "new-break");
        ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "posted") == 0 || strcmp(name, "net-zero-postings") == 0) {
        if (strcmp(name, "posted") == 0) {
            UmiBankCommand credit = Make(&f, UMI_BANK_TEST_CREDIT, "late"); Id(&credit.sourceAccountId, "account");
            credit.amount = Cash(1); Send(&f, &f.operator, &credit);
        } else {
            UmiBankCommand charge = Charge(&f, "charge", "reconciliation-fee"); Send(&f, &f.maker, &charge);
            ChargeStep(&f, UMI_BANK_CHARGE_APPROVE, &f.checker); ChargeStep(&f, UMI_BANK_CHARGE_POST, &f.operator);
            ChargeStep(&f, UMI_BANK_CHARGE_REVERSE, &f.operator); Balance(&f, 100000, 0);
        }
        c = Resolution(&f); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_BUSY);
    } else if (strcmp(name, "stale") == 0) {
        --c.expectedRevision; ReconciliationFail(&f, &f.operator, c, UMI_STATUS_BUSY);
    } else if (strcmp(name, "invalid-state") == 0) {
        Id(&c.id, "match"); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_STATE);
        c = Reopen(&f); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_STATE);
        c = Resolution(&f); ApplyResolution(&f, &c);
        c = Resolution(&f); ReconciliationFail(&f, &f.operator, c, UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "blocked") == 0 || strcmp(name, "hold") == 0) {
        UmiBankCommand change = Make(&f, strcmp(name, "blocked") == 0 ? UMI_BANK_ACCOUNT_SET_STATE : UMI_BANK_HOLD_PLACE,
            strcmp(name, "blocked") == 0 ? "account" : "hold");
        if (strcmp(name, "blocked") == 0) { change.state = UMI_BANK_RECORD_BLOCKED; Send(&f, &f.maker, &change); }
        else { Id(&change.sourceAccountId, "account"); change.amount = Cash(100); Send(&f, &f.operator, &change); }
        c = Resolution(&f); ApplyResolution(&f, &c);
    } else return 2;
    UmiBankOperationsDestroy(f.bank); return 0;
}
