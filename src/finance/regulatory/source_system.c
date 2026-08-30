/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/source_system.c
 *
 * PURPOSE:
 *   Register a source system and extraction criticality for governed data.
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

#include "umicom/finance/regulatory/source_system.h"

#include <string.h>

UmiStatus umi_reg_source_system_init(UmiSourceSystem *record, const char *source_id, const char *name, const char *owner_id, int critical)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(critical == 0 || critical == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->source_id, sizeof record->source_id, source_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->name, sizeof record->name, name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->owner_id, sizeof record->owner_id, owner_id);
    if (status != UMI_STATUS_OK) return status;
    record->critical = critical;
    return UMI_STATUS_OK;
}
