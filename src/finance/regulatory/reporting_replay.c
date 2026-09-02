/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_replay.c
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

#include "umicom/finance/regulatory/reporting_replay.h"

#include <string.h>

/*
 * Initialise reg reporting replay from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_reg_reporting_replay_init(UmiReportingReplay *record, const char *replay_id, const char *checkpoint_id, uint32_t from_step, int dry_run)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(dry_run == 0 || dry_run == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->replay_id, sizeof record->replay_id, replay_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->checkpoint_id, sizeof record->checkpoint_id, checkpoint_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->from_step = from_step;
    record->dry_run = dry_run;
    return UMI_STATUS_OK;
}
