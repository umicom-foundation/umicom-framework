/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/pnl_explain_report.h
 *
 * PURPOSE:
 *   Aggregate named P&L explain buckets and reconciliation residual.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PNL_EXPLAIN_REPORT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PNL_EXPLAIN_REPORT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/pnl_explain_bucket.h"
/**
 * Represent the enterprise pnl explain report data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterprisePnlExplainReport { char portfolio_id[UMI_ENTERPRISE_ID_CAPACITY]; double actual_pnl; UmiEnterprisePnlExplainBucket buckets[32U]; size_t count; } UmiEnterprisePnlExplainReport;
/* Initialise an empty report for one actual portfolio P&L. */
UmiStatus umi_enterprise_pnl_explain_report_init(UmiEnterprisePnlExplainReport *report,const char *portfolio_id,double actual_pnl);
/* Add a unique named explanation bucket. */
UmiStatus umi_enterprise_pnl_explain_report_add(UmiEnterprisePnlExplainReport *report,const UmiEnterprisePnlExplainBucket *bucket);
/* Calculate residual P&L not explained by the registered buckets. */
double umi_enterprise_pnl_explain_report_residual(const UmiEnterprisePnlExplainReport *report);

#ifdef __cplusplus
}
#endif

#endif
