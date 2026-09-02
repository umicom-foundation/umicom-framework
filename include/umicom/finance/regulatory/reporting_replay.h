/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_replay.h
 *
 * PURPOSE:
 *   Define a deterministic replay request from a regulatory reporting checkpoint.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_REPLAY_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_REPLAY_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the reporting replay data shared with callers of this public contract.
 */
typedef struct UmiReportingReplay {
    char replay_id[UMI_REG_ID_CAPACITY];
    char checkpoint_id[UMI_REG_ID_CAPACITY];
    uint32_t from_step;
    int dry_run;
} UmiReportingReplay;

/* Initialise one validated define a deterministic replay request from a regulatory reporting checkpoint. record. */
UmiStatus umi_reg_reporting_replay_init(UmiReportingReplay *record, const char *replay_id, const char *checkpoint_id, uint32_t from_step, int dry_run);

#ifdef __cplusplus
}
#endif

#endif
