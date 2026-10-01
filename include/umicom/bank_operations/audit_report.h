/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/audit_report.h
 * PURPOSE: Capture searchable accepted-command evidence and its original ledger journals.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_OPERATIONS_AUDIT_REPORT_H
#define UMICOM_BANK_OPERATIONS_AUDIT_REPORT_H
#include "umicom/bank_operations/operations.h"
#include "umicom/base/csv_document.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiBankAuditFamily {
    UMI_BANK_AUDIT_ALL=0, UMI_BANK_AUDIT_CUSTOMER, UMI_BANK_AUDIT_ACCOUNT,
    UMI_BANK_AUDIT_BENEFICIARY, UMI_BANK_AUDIT_FUNDING, UMI_BANK_AUDIT_TRANSFER,
    UMI_BANK_AUDIT_HOLD, UMI_BANK_AUDIT_CARD, UMI_BANK_AUDIT_INTEREST,
    UMI_BANK_AUDIT_CHARGE, UMI_BANK_AUDIT_RECONCILIATION
} UmiBankAuditFamily;
/** All filters intersect. IDs are optional exact case-sensitive bank IDs, not
 * substring or account-relationship searches. Family scopes repeated entity IDs.
 * action=0 accepts all actions. Zero revisions/dates are unbounded; bounds are
 * inclusive. Business dates may be backdated; output retains revision order.
 * A zero-initialized query captures every accepted event in the loaded service. */
typedef struct UmiBankAuditQuery {
    UmiFinancialId actorId, entityId, requestId;
    UmiBankAuditFamily family;
    UmiBankAction action;
    uint64_t firstRevision, lastRevision;
    UmiFinancialDate fromDate, toDate;
} UmiBankAuditQuery;
typedef struct UmiBankAuditSummary {
    UmiBankAuditQuery query;
    uint64_t revision;
    size_t totalEvents, count, journalCount;
    bool durable;
} UmiBankAuditSummary;
typedef struct UmiBankAuditRow {
    UmiBankAuditEvent event;
    size_t journalCount;
} UmiBankAuditRow;
typedef struct UmiBankAuditReport UmiBankAuditReport;
const char *UmiBankAuditFamilyName(UmiBankAuditFamily family);
UmiStatus UmiBankAuditActionFamily(UmiBankAction action, UmiBankAuditFamily *outFamily);
UmiStatus UmiBankAuditQueryValidate(const UmiBankAuditQuery *query);
/** Serial owner-thread read only: no reload, write, approval or execution.
 * Failure clears *outReport. Empty matches are valid, including unknown IDs.
 * Captures at most the service's 256 retained events and 256 journals on heap.
 * Journals join by accepted revision, never by a reused entity ID alone.
 * The owned snapshot survives later writes, reload and service destruction.
 * This is a local practice audit: failed attempts and repeated idempotent calls
 * are not new events; supplied actor identity is not proof of authentication. */
UmiStatus UmiBankAuditCapture(const UmiBankOperations *operations, const UmiBankAuditQuery *query, UmiBankAuditReport **outReport);
void UmiBankAuditDestroy(UmiBankAuditReport *report);
/** Reads copy data and leave output unchanged on failure. Index is within this
 * captured report, not a revision number or an index into the live service. */
UmiStatus UmiBankAuditReadSummary(const UmiBankAuditReport *report, UmiBankAuditSummary *out);
UmiStatus UmiBankAuditRowAt(const UmiBankAuditReport *report, size_t index, UmiBankAuditRow *out);
UmiStatus UmiBankAuditJournalAt(const UmiBankAuditReport *report, size_t eventIndex, size_t journalIndex, UmiBankJournal *out);
/** Text required size includes NUL. NULL/0 measures. Short buffers are cleared
 * and return CAPACITY_EXCEEDED with the required size. Other failures clear
 * output and required. Outputs must not alias the report or outRequired. */
UmiStatus UmiBankAuditDescribe(const UmiBankAuditReport *report, char *output, size_t capacity, size_t *outRequired);
UmiStatus UmiBankAuditDescribeEvent(const UmiBankAuditReport *report, size_t index, char *output, size_t capacity, size_t *outRequired);
/** Two explicit captured exports: accepted command rows, or journal lines.
 * Both carry capture/filter context and an empty-result summary. Exact minor
 * units remain integers with currency/scale. No cross-currency total is made.
 * CSV uses shared UTF-8 validation and formula-text escaping; failure clears
 * *outDocument and never exposes a partial export. */
UmiStatus UmiBankAuditExportCsv(const UmiBankAuditReport *report, UmiCsvDocument **outDocument);
UmiStatus UmiBankAuditExportJournalsCsv(const UmiBankAuditReport *report, UmiCsvDocument **outDocument);
#ifdef __cplusplus
}
#endif
#endif
