/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/log.h
 *
 * PURPOSE:
 *   Publish the public log contract for reusable Umicom applications.
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

/**
 * Emit one error for a failed operation through the existing diagnostic sink.
 * Success emits nothing. The operation is a public identifier of at most 96
 * ASCII letters, digits, dots, underscores or hyphens; invalid identifiers are
 * replaced by "unspecified-operation" rather than copied into the message.
 * Supply a fixed operation name, never a path, credential or user document.
 * Formatting uses bounded stack storage even when reporting allocation failure.
 * The sink receives borrowed message storage for the duration of this call.
 * A null logger or sink is allowed. This does not change the supplied status,
 * perform recovery, or make a failed constructor usable.
 */
void umi_log_status(UmiLogger *logger, const char *operation,
                    UmiStatus status, uint64_t correlation_id);

#ifdef __cplusplus
}
#endif

#endif
