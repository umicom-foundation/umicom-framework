/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_lineage.h
 *
 * PURPOSE:
 *   Capture reproducible market/model/source lineage for a valuation job.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_LINEAGE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_LINEAGE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseValuationLineage { char job_id[UMI_ENTERPRISE_ID_CAPACITY]; uint64_t market_fingerprint; char model_id[UMI_ENTERPRISE_ID_CAPACITY]; char source_revision[UMI_ENTERPRISE_ID_CAPACITY]; uint64_t lineage_fingerprint; } UmiEnterpriseValuationLineage;
/* Initialise deterministic lineage evidence for one valuation job. */
UmiStatus umi_enterprise_valuation_lineage_init(UmiEnterpriseValuationLineage *lineage,const char *job_id,uint64_t market_fingerprint,const char *model_id,const char *source_revision);

#ifdef __cplusplus
}
#endif

#endif
