/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_audit.h
 *
 * PURPOSE:
 *   Maintain ordered audit evidence for reporting operations.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_AUDIT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_AUDIT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportingAuditEvent { uint64_t sequence; char action[UMI_REG_NAME_CAPACITY]; uint64_t evidence_hash; } UmiReportingAuditEvent;
typedef struct UmiReportingAudit { UmiReportingAuditEvent events[UMI_REG_MAX_HISTORY]; size_t count; } UmiReportingAudit;
/* Initialise an empty regulatory audit stream. */
void umi_reg_reporting_audit_init(UmiReportingAudit *audit);
/* Append a strictly increasing sequence with immutable evidence fingerprint. */
UmiStatus umi_reg_reporting_audit_append(UmiReportingAudit *audit,uint64_t sequence,const char *action,uint64_t evidence_hash);

#ifdef __cplusplus
}
#endif

#endif
