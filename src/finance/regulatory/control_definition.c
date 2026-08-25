/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/control_definition.c
 *
 * PURPOSE:
 *   Define a reusable financial or reporting control and severity.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/control_definition.h"

#include <string.h>

UmiStatus umi_reg_control_definition_init(UmiControlDefinition *record, const char *control_id, const char *name, const char *owner_id, UmiRegSeverity severity)
{
    UmiStatus status = UMI_STATUS_OK;
    if (record == NULL || !(severity >= UMI_REG_SEVERITY_INFO && severity <= UMI_REG_SEVERITY_CRITICAL)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    status = umi_reg_copy_text(record->control_id, sizeof record->control_id, control_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->name, sizeof record->name, name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_reg_copy_text(record->owner_id, sizeof record->owner_id, owner_id);
    if (status != UMI_STATUS_OK) return status;
    record->severity = severity;
    return UMI_STATUS_OK;
}
