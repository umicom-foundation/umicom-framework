/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/pnl_explain_bucket.h
 *
 * PURPOSE:
 *   Represent one named P&L explanation bucket.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_PNL_EXPLAIN_BUCKET_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_PNL_EXPLAIN_BUCKET_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterprisePnlExplainBucket { char name[UMI_ENTERPRISE_NAME_CAPACITY]; double pnl; } UmiEnterprisePnlExplainBucket;
/* Initialise one finite named P&L explanation bucket. */
UmiStatus umi_enterprise_pnl_explain_bucket_init(UmiEnterprisePnlExplainBucket *bucket,const char *name,double pnl);

#ifdef __cplusplus
}
#endif

#endif
