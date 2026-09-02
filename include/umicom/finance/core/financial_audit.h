/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/financial_audit.h
 *
 * PURPOSE:
 *   Define auditable financial-domain evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_FINANCIAL_AUDIT_H
#define UMICOM_FINANCE_CORE_FINANCIAL_AUDIT_H

#include "umicom/finance/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the financial audit record data shared with callers of this public contract.
 */
typedef struct UmiFinancialAuditRecord { UmiFinancialId audit_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; UmiFinancialDate effective_date; uint32_t state; bool active; } UmiFinancialAuditRecord;
/* Initialize the typed financial record. */ UmiStatus umi_financial_audit_init(UmiFinancialAuditRecord *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state);
/* Validate the typed financial record. */ bool umi_financial_audit_is_valid(const UmiFinancialAuditRecord *item);
#ifdef __cplusplus
}
#endif

#endif
