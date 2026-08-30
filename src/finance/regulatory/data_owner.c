/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_owner.c
 *
 * PURPOSE:
 *   Record accountable business ownership for a governed data domain.
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

#include "umicom/finance/regulatory/data_owner.h"

#include <string.h>

UmiStatus umi_reg_data_owner_init(UmiDataOwner *record, const char *owner_id, const char *domain_id, const char *display_name, int active)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(active == 0 || active == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->owner_id, sizeof record->owner_id, owner_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->domain_id, sizeof record->domain_id, domain_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->display_name, sizeof record->display_name, display_name);
    if (status != UMI_STATUS_OK) return status;
    record->active = active;
    return UMI_STATUS_OK;
}
