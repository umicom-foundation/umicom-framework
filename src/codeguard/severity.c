/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/severity.c
 *
 * PURPOSE:
 *   Convert CodeGuard severity values into stable lowercase names.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/severity.h"
/*
 * Provide the codeguard severity text operation used by this module and its client
 * applications.
 */
const char *umi_codeguard_severity_text(UmiCodeGuardSeverity severity)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (severity) {
        case UMI_CODEGUARD_INFO: return "info";
        case UMI_CODEGUARD_LOW: return "low";
        case UMI_CODEGUARD_MEDIUM: return "medium";
        case UMI_CODEGUARD_HIGH: return "high";
        case UMI_CODEGUARD_CRITICAL: return "critical";
        default: return "unknown";
    }
}
