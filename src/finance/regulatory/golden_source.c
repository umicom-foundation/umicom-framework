/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/golden_source.c
 *
 * PURPOSE:
 *   Designate an authoritative golden source for a governed data domain.
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

#include "umicom/finance/regulatory/golden_source.h"

#include <string.h>

UmiStatus umi_reg_golden_source_init(UmiGoldenSource *record, const char *domain_id, const char *source_id, uint32_t priority, int active)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(priority > 0U && (active == 0 || active == 1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->domain_id, sizeof record->domain_id, domain_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->source_id, sizeof record->source_id, source_id);
    if (status != UMI_STATUS_OK) return status;
    record->priority = priority;
    record->active = active;
    return UMI_STATUS_OK;
}
