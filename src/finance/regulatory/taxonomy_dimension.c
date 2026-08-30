/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/taxonomy_dimension.c
 *
 * PURPOSE:
 *   Describe one reporting dimension used to qualify taxonomy facts.
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

#include "umicom/finance/regulatory/taxonomy_dimension.h"

#include <string.h>

UmiStatus umi_reg_taxonomy_dimension_init(UmiTaxonomyDimension *record, const char *dimension_id, const char *taxonomy_id, const char *label, int required)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(required == 0 || required == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->dimension_id, sizeof record->dimension_id, dimension_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->taxonomy_id, sizeof record->taxonomy_id, taxonomy_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->label, sizeof record->label, label);
    if (status != UMI_STATUS_OK) return status;
    record->required = required;
    return UMI_STATUS_OK;
}
