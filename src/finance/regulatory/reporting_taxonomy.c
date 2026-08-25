/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_taxonomy.c
 *
 * PURPOSE:
 *   Describe a versioned reporting taxonomy controlled by an authority.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/reporting_taxonomy.h"

#include <string.h>

UmiStatus umi_reg_reporting_taxonomy_init(UmiReportingTaxonomy *record, const char *taxonomy_id, const char *authority_id, const char *version, int64_t effective_from_day)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(effective_from_day >= 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->taxonomy_id, sizeof record->taxonomy_id, taxonomy_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->authority_id, sizeof record->authority_id, authority_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->version, sizeof record->version, version);
    if (status != UMI_STATUS_OK) return status;
    record->effective_from_day = effective_from_day;
    return UMI_STATUS_OK;
}
