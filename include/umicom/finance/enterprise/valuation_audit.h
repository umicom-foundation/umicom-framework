/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_audit.h
 *
 * PURPOSE:
 *   Record ordered valuation control-plane events for operational audit.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_AUDIT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_AUDIT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise valuation audit event data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationAuditEvent { uint64_t sequence; char job_id[UMI_ENTERPRISE_ID_CAPACITY]; char action[UMI_ENTERPRISE_NAME_CAPACITY]; int64_t event_time_ms; UmiStatus status; } UmiEnterpriseValuationAuditEvent;
/**
 * Represent the enterprise valuation audit data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationAudit { UmiEnterpriseValuationAuditEvent events[UMI_ENTERPRISE_MAX_ITEMS]; size_t count; } UmiEnterpriseValuationAudit;
/* Initialise an empty valuation audit. */
void umi_enterprise_valuation_audit_init(UmiEnterpriseValuationAudit *audit);
/* Append strictly ordered valuation audit evidence. */
UmiStatus umi_enterprise_valuation_audit_append(UmiEnterpriseValuationAudit *audit,const UmiEnterpriseValuationAuditEvent *event);

#ifdef __cplusplus
}
#endif

#endif
