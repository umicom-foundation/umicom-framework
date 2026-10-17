/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/log.c
 *
 * PURPOSE:
 *   Implement the log behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/log.h"

#include <stdio.h>

/* Provide the log operation used by this module and its client applications. */
void umi_log(UmiLogger *logger, UmiDiagnosticSeverity severity,
             const char *message, uint64_t correlation_id)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (logger == 0) {
        return;
    }
    umi_diagnostic_emit(logger->sink, logger->user_data, severity,
                        logger->source, message, correlation_id);
}

/* Only a small public operation identifier is accepted. Keeping formatting
 * here gives every application the same status evidence without encouraging
 * callers to embed private paths or arbitrary peer messages in startup logs. */
static int log_operation_valid(const char *operation)
{
    if (operation == NULL || operation[0] == '\0') return 0;
    for (size_t index = 0U; index <= 96U; ++index) {
        unsigned char value = (unsigned char)operation[index];
        if (value == '\0') return 1;
        if (index == 96U || !((value >= 'a' && value <= 'z') ||
            (value >= 'A' && value <= 'Z') || (value >= '0' && value <= '9') ||
            value == '.' || value == '_' || value == '-')) return 0;
    }
    return 0;
}

void umi_log_status(UmiLogger *logger, const char *operation,
                    UmiStatus status, uint64_t correlation_id)
{
    if (logger == NULL || logger->sink == NULL || status == UMI_STATUS_OK) return;
    /* No allocation or sink invocation occurs before the message is complete.
     * Its bounded fields also make allocation failures reportable. */
    char message[192];
    const char *name = log_operation_valid(operation) ? operation : "unspecified-operation";
    int written = snprintf(message, sizeof message, "Operation %s failed: %s (%d).",
        name, umi_status_text(status), (int)status);
    if (written < 0 || (size_t)written >= sizeof message) return;
    umi_log(logger, UMI_DIAGNOSTIC_ERROR, message, correlation_id);
}
