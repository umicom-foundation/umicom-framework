/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_result_store.h
 *
 * PURPOSE:
 *   Persist bounded instrument-level valuation results for aggregation and replay.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_RESULT_STORE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_RESULT_STORE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise valuation result record data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationResultRecord { char job_id[UMI_ENTERPRISE_ID_CAPACITY]; char task_id[UMI_ENTERPRISE_ID_CAPACITY]; double present_value; UmiStatus status; } UmiEnterpriseValuationResultRecord;
/**
 * Represent the enterprise valuation result store data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationResultStore { UmiEnterpriseValuationResultRecord records[UMI_ENTERPRISE_MAX_ITEMS]; size_t count; } UmiEnterpriseValuationResultStore;
/* Initialise an empty valuation-result store. */
void umi_enterprise_valuation_result_store_init(UmiEnterpriseValuationResultStore *store);
/* Append one unique task result. */
UmiStatus umi_enterprise_valuation_result_store_append(UmiEnterpriseValuationResultStore *store,const UmiEnterpriseValuationResultRecord *record);
/* Sum successful present values for a named job. */
double umi_enterprise_valuation_result_store_job_pv(const UmiEnterpriseValuationResultStore *store,const char *job_id);

#ifdef __cplusplus
}
#endif

#endif
