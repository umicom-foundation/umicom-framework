/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/report_section.c
 *
 * PURPOSE:
 *   Define one ordered section in a regulatory report template.
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

#include "umicom/finance/regulatory/report_section.h"

#include <string.h>

UmiStatus umi_reg_report_section_init(UmiReportSection *record, const char *section_id, const char *template_id, const char *title, uint32_t ordinal)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(ordinal > 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->section_id, sizeof record->section_id, section_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->template_id, sizeof record->template_id, template_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->title, sizeof record->title, title);
    if (status != UMI_STATUS_OK) return status;
    record->ordinal = ordinal;
    return UMI_STATUS_OK;
}
