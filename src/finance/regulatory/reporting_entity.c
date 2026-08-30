/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_entity.c
 *
 * PURPOSE:
 *   Represent one legal reporting entity and consolidation perimeter.
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

#include "umicom/finance/regulatory/reporting_entity.h"

#include <string.h>

UmiStatus umi_reg_reporting_entity_init(UmiReportingEntity *record, const char *entity_id, const char *legal_name, const char *jurisdiction, int consolidated)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(consolidated == 0 || consolidated == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->entity_id, sizeof record->entity_id, entity_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->legal_name, sizeof record->legal_name, legal_name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->jurisdiction, sizeof record->jurisdiction, jurisdiction);
    if (status != UMI_STATUS_OK) return status;
    record->consolidated = consolidated;
    return UMI_STATUS_OK;
}
