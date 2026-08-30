/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_checkpoint.c
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

#include "umicom/finance/regulatory/reporting_checkpoint.h"

#include <string.h>

UmiStatus umi_reg_reporting_checkpoint_init(UmiReportingCheckpoint *record, const char *checkpoint_id, const char *job_id, uint32_t completed_steps, uint64_t state_hash)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(state_hash != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->checkpoint_id, sizeof record->checkpoint_id, checkpoint_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->job_id, sizeof record->job_id, job_id);
    if (status != UMI_STATUS_OK) return status;
    record->completed_steps = completed_steps;
    record->state_hash = state_hash;
    return UMI_STATUS_OK;
}
