/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/report_template.c
 *
 * PURPOSE:
 *   Define a versioned report template tied to one taxonomy.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/report_template.h"

#include <string.h>

UmiStatus umi_reg_report_template_init(UmiReportTemplate *record, const char *template_id, const char *taxonomy_id, const char *name, uint32_t version)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(version > 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->template_id, sizeof record->template_id, template_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->taxonomy_id, sizeof record->taxonomy_id, taxonomy_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->name, sizeof record->name, name);
    if (status != UMI_STATUS_OK) return status;
    record->version = version;
    return UMI_STATUS_OK;
}
