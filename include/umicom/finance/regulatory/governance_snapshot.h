/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/governance_snapshot.h
 *
 * PURPOSE:
 *   Aggregate governance, quality, lineage and control readiness metrics.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_GOVERNANCE_SNAPSHOT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_GOVERNANCE_SNAPSHOT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the governance snapshot data shared with callers of this public contract.
 */
typedef struct UmiGovernanceSnapshot { size_t domains; size_t lineage_nodes; size_t controls; double quality_score; size_t open_exceptions; } UmiGovernanceSnapshot;
/* Initialise a finite aggregate governance snapshot. */
UmiStatus umi_reg_governance_snapshot_init(UmiGovernanceSnapshot *snapshot,size_t domains,size_t lineage_nodes,size_t controls,double quality_score,size_t open_exceptions);
/* Return one when quality passes threshold and no open critical exceptions remain. */
int umi_reg_governance_snapshot_ready(const UmiGovernanceSnapshot *snapshot,double minimum_quality);

#ifdef __cplusplus
}
#endif

#endif
