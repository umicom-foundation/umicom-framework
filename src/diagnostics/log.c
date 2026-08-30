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

void umi_log(UmiLogger *logger, UmiDiagnosticSeverity severity,
             const char *message, uint64_t correlation_id)
{
    if (logger == 0) {
        return;
    }
    umi_diagnostic_emit(logger->sink, logger->user_data, severity,
                        logger->source, message, correlation_id);
}
