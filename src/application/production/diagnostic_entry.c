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

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Copy application production diagnostic entry into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_application_production_diagnostic_entry_set(
    UmiApplicationProductionDiagnosticEntry *entry,
    UmiApplicationProductionDiagnosticSeverity severity,
    const char *code, const char *subject_id, const char *message)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || severity < UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_INFO ||
        severity > UMI_APPLICATION_PRODUCTION_DIAGNOSTIC_ERROR)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(entry, 0, sizeof(*entry));
    status = copy_text(entry->code, sizeof(entry->code), code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(entry->subject_id, sizeof(entry->subject_id),
                       subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(entry->message, sizeof(entry->message), message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    entry->severity = severity;
    return UMI_STATUS_OK;
}

