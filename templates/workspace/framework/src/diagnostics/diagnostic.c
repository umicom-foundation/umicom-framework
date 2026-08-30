/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/diagnostics/diagnostic.c
 *
 * PURPOSE:
 *   Implement the diagnostic behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/diagnostic.h"

const char *umi_diagnostic_severity_text(UmiDiagnosticSeverity severity)
{
    switch (severity) {
        case UMI_DIAGNOSTIC_TRACE: return "TRACE";
        case UMI_DIAGNOSTIC_INFO: return "INFO";
        case UMI_DIAGNOSTIC_WARNING: return "WARNING";
        case UMI_DIAGNOSTIC_ERROR: return "ERROR";
        case UMI_DIAGNOSTIC_FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

void umi_diagnostic_emit(UmiDiagnosticSink sink, void *user_data,
                         UmiDiagnosticSeverity severity, const char *source,
                         const char *message, uint64_t correlation_id)
{
    UmiDiagnostic diagnostic;
    if (sink == 0) {
        return;
    }
    diagnostic.severity = severity;
    diagnostic.source = source != 0 ? source : "unknown";
    diagnostic.message = message != 0 ? message : "";
    diagnostic.correlation_id = correlation_id;
    sink(&diagnostic, user_data);
}
