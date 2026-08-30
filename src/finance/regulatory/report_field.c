/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/report_field.c
 *
 * PURPOSE:
 *   Map a report field to a taxonomy concept and section.
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

#include "umicom/finance/regulatory/report_field.h"

#include <string.h>

UmiStatus umi_reg_report_field_init(UmiReportField *record, const char *field_id, const char *section_id, const char *concept_id, int mandatory)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(mandatory == 0 || mandatory == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->field_id, sizeof record->field_id, field_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->section_id, sizeof record->section_id, section_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->concept_id, sizeof record->concept_id, concept_id);
    if (status != UMI_STATUS_OK) return status;
    record->mandatory = mandatory;
    return UMI_STATUS_OK;
}
