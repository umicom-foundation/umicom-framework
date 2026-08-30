/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_steward.c
 *
 * PURPOSE:
 *   Record operational stewardship responsibility for governed data.
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

#include "umicom/finance/regulatory/data_steward.h"

#include <string.h>

UmiStatus umi_reg_data_steward_init(UmiDataSteward *record, const char *steward_id, const char *domain_id, const char *display_name, int active)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(active == 0 || active == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->steward_id, sizeof record->steward_id, steward_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->domain_id, sizeof record->domain_id, domain_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->display_name, sizeof record->display_name, display_name);
    if (status != UMI_STATUS_OK) return status;
    record->active = active;
    return UMI_STATUS_OK;
}
