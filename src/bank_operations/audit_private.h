/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_operations/audit_private.h
 * PURPOSE: Own immutable audit rows and flat journal storage independently of the service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_AUDIT_PRIVATE_H
#define UMICOM_BANK_AUDIT_PRIVATE_H
#include "umicom/bank_operations/audit_report.h"
/* Reuse the existing validated Gregorian-date display helper. */
#include "activity_private.h"
struct UmiBankAuditReport {
    UmiBankAuditSummary summary;
    UmiBankAuditRow rows[UMI_BANK_EVENT_CAPACITY];
    size_t journalStart[UMI_BANK_EVENT_CAPACITY];
    UmiBankJournal journals[UMI_BANK_EVENT_CAPACITY];
};
int BankAuditMatches(const UmiBankAuditQuery *query,const UmiBankAuditEvent *event);
#endif
