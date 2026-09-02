/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_replay.h
 *
 * PURPOSE:
 *   Store valuation checkpoints and resolve the latest recoverable state per job.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_REPLAY_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_REPLAY_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/valuation_checkpoint.h"
/**
 * Represent the enterprise valuation replay data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationReplay { UmiEnterpriseValuationCheckpoint checkpoints[64U]; size_t count; } UmiEnterpriseValuationReplay;
/* Initialise an empty replay checkpoint store. */
void umi_enterprise_valuation_replay_init(UmiEnterpriseValuationReplay *replay);
/* Append a checkpoint while enforcing monotonic job progress. */
UmiStatus umi_enterprise_valuation_replay_append(UmiEnterpriseValuationReplay *replay,const UmiEnterpriseValuationCheckpoint *checkpoint);
/* Return the latest checkpoint for one job. */
const UmiEnterpriseValuationCheckpoint *umi_enterprise_valuation_replay_latest(const UmiEnterpriseValuationReplay *replay,const char *job_id);

#ifdef __cplusplus
}
#endif

#endif
