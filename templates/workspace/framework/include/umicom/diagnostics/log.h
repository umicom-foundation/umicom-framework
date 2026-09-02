/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/include/umicom/diagnostics/log.h
 *
 * PURPOSE:
 *   Declare the log contract shared by Framework services and thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DIAGNOSTICS_LOG_H
#define UMICOM_DIAGNOSTICS_LOG_H

#include "umicom/diagnostics/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the logger data shared with callers of this public contract.
 */
typedef struct UmiLogger {
    UmiDiagnosticSink sink;
    void *user_data;
    const char *source;
} UmiLogger;

/**
 * Provide the log operation used by this module and its client applications.
 */
void umi_log(UmiLogger *logger, UmiDiagnosticSeverity severity,
             const char *message, uint64_t correlation_id);

#ifdef __cplusplus
}
#endif

#endif
