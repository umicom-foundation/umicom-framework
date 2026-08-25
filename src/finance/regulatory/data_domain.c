/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/data_domain.c
 *
 * PURPOSE:
 *   Describe a governed financial data domain.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/data_domain.h"

#include <string.h>

UmiStatus umi_reg_data_domain_init(UmiDataDomain *record, const char *domain_id, const char *name, const char *classification, int regulated)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(regulated == 0 || regulated == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->domain_id, sizeof record->domain_id, domain_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->name, sizeof record->name, name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->classification, sizeof record->classification, classification);
    if (status != UMI_STATUS_OK) return status;
    record->regulated = regulated;
    return UMI_STATUS_OK;
}
