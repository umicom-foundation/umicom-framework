/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_reconciliation/fixture.h
 * PURPOSE: Share exact reconciliation observations and reviewed transitions over the canonical bank.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_RECONCILIATION_TEST_FIXTURE_H
#define UMICOM_BANK_RECONCILIATION_TEST_FIXTURE_H
#include "../bank_charges/fixture.h"
#include "umicom/bank_operations/reconciliation.h"

/* Comparisons are normal recorded commands, not injected balance snapshots. */
static inline void Compare(Fixture *f, const char *id, const char *account, int64_t minor)
{
    UmiBankCommand c = Make(f, UMI_BANK_RECONCILE, id);
    Id(&c.sourceAccountId, account); c.amount = Cash(minor); Send(f, &f->operator, &c);
}
static inline void ReconciliationSetup(Fixture *f)
{
    Setup(f); Compare(f, "break", "account", 90000); Compare(f, "match", "account", 100000);
}
static inline UmiBankCommand Resolution(Fixture *f)
{
    UmiBankCommand c = Make(f, UMI_BANK_RECONCILIATION_RESOLVE, "break");
    Id(&c.ownerId, "match"); strcpy(c.name, "Investigated statement timing difference"); return c;
}
static inline UmiBankCommand Reopen(Fixture *f)
{
    UmiBankCommand c = Make(f, UMI_BANK_RECONCILIATION_REOPEN, "break");
    strcpy(c.name, "New information requires investigation"); return c;
}
static inline UmiBankReconciliation ReadBreak(Fixture *f)
{
    UmiBankReconciliation record; OK(UmiBankOperationsFindReconciliation(f->bank, "break", &record)); return record;
}
/* A disposition changes only one retained investigation and its audit event. */
static inline void ApplyResolution(Fixture *f, const UmiBankCommand *command)
{
    UmiBankCounts before, after; UmiBankBalance money, actual; UmiBankReceipt receipt;
    OK(UmiBankOperationsCounts(f->bank, &before)); OK(UmiBankOperationsBalance(f->bank, "account", &money));
    UmiBankReview *review = NULL; OK(UmiBankOperationsReview(f->bank, &f->operator, command, &review));
    OK(UmiBankOperationsExecuteReviewed(f->bank, &f->operator, review, &receipt));
    CHECK(receipt.revision == before.revision + 1U && !receipt.idempotent); UmiBankReviewDestroy(review);
    OK(UmiBankOperationsCounts(f->bank, &after)); OK(UmiBankOperationsBalance(f->bank, "account", &actual));
    CHECK(after.journals == before.journals && after.reconciliations == before.reconciliations && after.events == before.events + 1U);
    CHECK(actual.booked.minor_units == money.booked.minor_units && actual.reserved.minor_units == money.reserved.minor_units &&
        actual.available.minor_units == money.available.minor_units);
}
static inline void ReconciliationFail(Fixture *f, const UmiBankActor *actor, UmiBankCommand command, UmiStatus expected)
{
    UmiBankReconciliation before = ReadBreak(f);
    Fail(f, actor, command, expected);
    UmiBankReconciliation after = ReadBreak(f);
    CHECK(after.disposition == before.disposition && after.reviewedRevision == before.reviewedRevision &&
        strcmp(after.evidenceId.value, before.evidenceId.value) == 0 && strcmp(after.reviewReason, before.reviewReason) == 0);
}
#endif
