#ifndef UMICOM_DIAGNOSTICS_LOG_H
#define UMICOM_DIAGNOSTICS_LOG_H

#include "umicom/diagnostics/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLogger {
    UmiDiagnosticSink sink;
    void *user_data;
    const char *source;
} UmiLogger;

void umi_log(UmiLogger *logger, UmiDiagnosticSeverity severity,
             const char *message, uint64_t correlation_id);

#ifdef __cplusplus
}
#endif

#endif
