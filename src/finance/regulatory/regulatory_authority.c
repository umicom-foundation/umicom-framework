/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/regulatory_authority.c
 *
 * PURPOSE:
 *   Represent a regulatory authority and jurisdiction boundary.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/regulatory_authority.h"

#include <string.h>

UmiStatus umi_reg_regulatory_authority_init(UmiRegulatoryAuthority *record, const char *authority_id, const char *name, const char *jurisdiction, int active)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(active == 0 || active == 1)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->authority_id, sizeof record->authority_id, authority_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->name, sizeof record->name, name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->jurisdiction, sizeof record->jurisdiction, jurisdiction);
    if (status != UMI_STATUS_OK) return status;
    record->active = active;
    return UMI_STATUS_OK;
}
