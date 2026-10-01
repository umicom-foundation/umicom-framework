/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/reservations.h
 * PURPOSE: Explain a captured available balance and review one explicit reservation release.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_BANK_OPERATIONS_RESERVATIONS_H
#define UMICOM_BANK_OPERATIONS_RESERVATIONS_H
#include "umicom/bank_operations/review.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BANK_RESERVATION_CAPACITY (2U * UMI_BANK_RECORD_CAPACITY)
typedef enum UmiBankReservationKind {
    UMI_BANK_RESERVATION_MANUAL = 1,
    UMI_BANK_RESERVATION_CARD = 2,
    UMI_BANK_RESERVATION_TRANSFER = 3
} UmiBankReservationKind;
/** All rows are active reservations. Kind plus ID is the identity: a transfer
 * and a hold may use the same ID. Transfer state is pending or approved; the
 * field is zero for holds. Reserved money is not a posted debit. */
typedef struct UmiBankReservationRow {
    UmiBankReservationKind kind;
    UmiFinancialId id, accountId, cardId, destinationAccountId, referenceId, makerId;
    UmiMoney amount;
    UmiBankTransferState transferState;
    uint64_t createdRevision;
} UmiBankReservationRow;
typedef struct UmiBankReservationsSummary {
    UmiFinancialId accountId;
    UmiBankBalance balance;
    uint64_t revision;
    size_t count, manualCount, cardCount, transferCount;
    int64_t manualMinor, cardMinor, transferMinor;
    bool durable;
} UmiBankReservationsSummary;
/** Immutable local-practice evidence. The three category totals must exactly
 * explain the canonical reserved balance. Charges and interest do not reserve
 * funds and are excluded. There is no reload, write, automatic expiry or
 * external authorisation lookup. Capture on the service's serial owner thread.
 * The owned report remains readable after the service closes. */
typedef struct UmiBankReservations UmiBankReservations;
UmiStatus UmiBankReservationsCapture(const UmiBankOperations *operations,
    const char *accountId, UmiBankReservations **outReport);
void UmiBankReservationsDestroy(UmiBankReservations *report);
/** Copied outputs remain unchanged on failure. Unknown accounts return
 * NOT_FOUND; a known account with no reservations is a valid empty report. */
UmiStatus UmiBankReservationsReadSummary(const UmiBankReservations *report, UmiBankReservationsSummary *outSummary);
UmiStatus UmiBankReservationsRowAt(const UmiBankReservations *report, size_t index, UmiBankReservationRow *outRow);
const char *UmiBankReservationKindName(UmiBankReservationKind kind);
/** Resolve manual release, card void or transfer cancellation. This names the
 * action but supplies no authority and executes nothing. Output is unchanged
 * on failure. Captured rows remain ordered by creation revision. */
UmiStatus UmiBankReservationsReleaseAction(const UmiBankReservations *report, size_t index, UmiBankAction *outAction);
/** Require the exact captured history, row ID, release action and revision.
 * Trusted caller-supplied capabilities and canonical ownership controls still
 * apply. Equivalent reloaded history is allowed; stale or foreign history is
 * BUSY. Failure clears *outReview. Only a separate ExecuteReviewed decision
 * can publish the command. Execution still rechecks repository concurrency. */
UmiStatus UmiBankReservationsReviewRelease(const UmiBankOperations *operations,
    const UmiBankReservations *report, size_t index, const UmiBankActor *actor,
    const UmiBankCommand *command, UmiBankReview **outReview);
/** Describe/export the exact capture. Text NULL/0 measures; required includes
 * NUL. Short output is cleared and returns CAPACITY_EXCEEDED with required size.
 * Other errors clear output and set required=0. Output cannot alias inputs.
 * CSV amounts are integer minor units with explicit currency and scale. */
UmiStatus UmiBankReservationsDescribe(const UmiBankReservations *report,
    char *output, size_t capacity, size_t *outRequired);
UmiStatus UmiBankReservationsExportCsv(const UmiBankReservations *report, UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
