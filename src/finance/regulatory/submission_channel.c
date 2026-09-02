/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/submission_channel.c
 *
 * PURPOSE:
 *   Describe one approved transport channel for regulatory submissions.
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

#include "umicom/finance/regulatory/submission_channel.h"

#include <string.h>

/*
 * Initialise reg submission channel from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_submission_channel_init(UmiSubmissionChannel *record, const char *channel_id, const char *authority_id, const char *protocol, int enabled)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(enabled == 0 || enabled == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->channel_id, sizeof record->channel_id, channel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->authority_id, sizeof record->authority_id, authority_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->protocol, sizeof record->protocol, protocol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->enabled = enabled;
    return UMI_STATUS_OK;
}
