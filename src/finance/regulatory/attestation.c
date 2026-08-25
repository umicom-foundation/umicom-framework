/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/attestation.c
 *
 * PURPOSE:
 *   Record management attestation and signed-evidence reference.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/attestation.h"

#include <string.h>

UmiStatus umi_reg_attestation_init(UmiRegAttestation *record, const char *attestation_id, const char *report_id, const char *signer_id, uint64_t evidence_hash)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(evidence_hash != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->attestation_id, sizeof record->attestation_id, attestation_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->report_id, sizeof record->report_id, report_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->signer_id, sizeof record->signer_id, signer_id);
    if (status != UMI_STATUS_OK) return status;
    record->evidence_hash = evidence_hash;
    return UMI_STATUS_OK;
}
