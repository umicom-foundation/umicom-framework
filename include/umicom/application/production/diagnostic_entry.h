/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/diagnostic_entry.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_DIAGNOSTIC_ENTRY_H
#define UMICOM_APPLICATION_PRODUCTION_DIAGNOSTIC_ENTRY_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/types.h"

/**
 * Represent the application production diagnostic entry data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationProductionDiagnosticEntry {
    UmiApplicationProductionDiagnosticSeverity severity;
    char code[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char subject_id[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
    char message[UMI_APPLICATION_PRODUCTION_TEXT_CAPACITY];
} UmiApplicationProductionDiagnosticEntry;

/**
 * Copy application production diagnostic entry into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_application_production_diagnostic_entry_set(
    UmiApplicationProductionDiagnosticEntry *entry,
    UmiApplicationProductionDiagnosticSeverity severity,
    const char *code, const char *subject_id, const char *message);

#ifdef __cplusplus
}
#endif
#endif
