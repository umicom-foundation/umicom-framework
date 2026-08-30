/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/diagnostic_entry.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/diagnostic_entry.h"

#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_application_production_diagnostic_entry_set(
    UmiApplicationProductionDiagnosticEntry *entry,
    UmiApplicationProductionDiagnosticSeverity severity,
    const char *code, const char *subject_id, const char *message)
{
    UmiStatus status;
    if (entry == NULL || severity < UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_INFO ||
        severity > UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_ERROR)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(entry, 0, sizeof(*entry));
    status = copy_text(entry->code, sizeof(entry->code), code);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(entry->subject_id, sizeof(entry->subject_id),
                       subject_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(entry->message, sizeof(entry->message), message);
    if (status != UMI_STATUS_OK) return status;
    entry->severity = severity;
    return UMI_STATUS_OK;
}

