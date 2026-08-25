/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_evidence.c
 *
 * PURPOSE:
 *   Represent immutable source evidence supporting a report fact.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/reporting_evidence.h"

#include <string.h>

UmiStatus umi_reg_reporting_evidence_init(UmiReportingEvidence *record, const char *evidence_id, const char *source_id, const char *locator, uint64_t content_hash)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(content_hash != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->evidence_id, sizeof record->evidence_id, evidence_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->source_id, sizeof record->source_id, source_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->locator, sizeof record->locator, locator);
    if (status != UMI_STATUS_OK) return status;
    record->content_hash = content_hash;
    return UMI_STATUS_OK;
}
