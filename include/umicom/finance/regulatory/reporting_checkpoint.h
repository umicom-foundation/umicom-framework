/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_checkpoint.h
 *
 * PURPOSE:
 *   Capture recoverable reporting progress and immutable evidence fingerprint.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_CHECKPOINT_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_CHECKPOINT_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the reporting checkpoint data shared with callers of this public contract.
 */
typedef struct UmiReportingCheckpoint {
    char checkpoint_id[UMI_REG_ID_CAPACITY];
    char job_id[UMI_REG_ID_CAPACITY];
    uint32_t completed_steps;
    uint64_t state_hash;
} UmiReportingCheckpoint;

/* Initialise one validated capture recoverable reporting progress and immutable evidence fingerprint. record. */
UmiStatus umi_reg_reporting_checkpoint_init(UmiReportingCheckpoint *record, const char *checkpoint_id, const char *job_id, uint32_t completed_steps, uint64_t state_hash);

#ifdef __cplusplus
}
#endif

#endif
