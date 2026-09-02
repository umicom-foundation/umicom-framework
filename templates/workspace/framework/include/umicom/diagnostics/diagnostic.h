/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/diagnostics/diagnostic.h
 *
 * PURPOSE:
 *   Declare the diagnostic contract shared by Framework services and thin
 *   applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTICS_DIAGNOSTIC_H
#define UMICOM_DIAGNOSTICS_DIAGNOSTIC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named diagnostic severity values accepted by this public contract.
 */
typedef enum UmiDiagnosticSeverity {
    UMI_DIAGNOSTIC_TRACE = 0,
    UMI_DIAGNOSTIC_INFO = 1,
    UMI_DIAGNOSTIC_WARNING = 2,
    UMI_DIAGNOSTIC_ERROR = 3,
    UMI_DIAGNOSTIC_FATAL = 4
} UmiDiagnosticSeverity;

/**
 * Represent the diagnostic data shared with callers of this public contract.
 */
typedef struct UmiDiagnostic {
    UmiDiagnosticSeverity severity;
    const char *source;
    const char *message;
    uint64_t correlation_id;
} UmiDiagnostic;

typedef void (*UmiDiagnosticSink)(const UmiDiagnostic *diagnostic, void *user_data);

/**
 * Provide the diagnostic severity text operation used by this module and its client
 * applications.
 */
const char *umi_diagnostic_severity_text(UmiDiagnosticSeverity severity);
/**
 * Provide the diagnostic emit operation used by this module and its client applications.
 */
void umi_diagnostic_emit(UmiDiagnosticSink sink, void *user_data,
                         UmiDiagnosticSeverity severity, const char *source,
                         const char *message, uint64_t correlation_id);

#ifdef __cplusplus
}
#endif

#endif
