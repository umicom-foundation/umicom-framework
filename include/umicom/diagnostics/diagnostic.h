#ifndef UMICOM_DIAGNOSTICS_DIAGNOSTIC_H
#define UMICOM_DIAGNOSTICS_DIAGNOSTIC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDiagnosticSeverity {
    UMI_DIAGNOSTIC_TRACE = 0,
    UMI_DIAGNOSTIC_INFO = 1,
    UMI_DIAGNOSTIC_WARNING = 2,
    UMI_DIAGNOSTIC_ERROR = 3,
    UMI_DIAGNOSTIC_FATAL = 4
} UmiDiagnosticSeverity;

typedef struct UmiDiagnostic {
    UmiDiagnosticSeverity severity;
    const char *source;
    const char *message;
    uint64_t correlation_id;
} UmiDiagnostic;

typedef void (*UmiDiagnosticSink)(const UmiDiagnostic *diagnostic, void *user_data);

const char *umi_diagnostic_severity_text(UmiDiagnosticSeverity severity);
void umi_diagnostic_emit(UmiDiagnosticSink sink, void *user_data,
                         UmiDiagnosticSeverity severity, const char *source,
                         const char *message, uint64_t correlation_id);

#ifdef __cplusplus
}
#endif

#endif
