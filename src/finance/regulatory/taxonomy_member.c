/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/taxonomy_member.c
 *
 * PURPOSE:
 *   Describe an allowed member of a reporting dimension.
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

#include "umicom/finance/regulatory/taxonomy_member.h"

#include <string.h>

/*
 * Initialise reg taxonomy member from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_reg_taxonomy_member_init(UmiTaxonomyMember *record, const char *member_id, const char *dimension_id, const char *label, int active)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || !(active == 0 || active == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->member_id, sizeof record->member_id, member_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->dimension_id, sizeof record->dimension_id, dimension_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->label, sizeof record->label, label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->active = active;
    return UMI_STATUS_OK;
}
