/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/diagnostics_pull_state.h
 *
 * PURPOSE:
 *   Track pull-diagnostics lifecycle and document generation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_PULL_STATE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_PULL_STATE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_PULL_STATE_API_VERSION 1U
/**
 * Represent the language intelligence diagnostics pull state data shared with callers of
 * this public contract.
 */
typedef struct UmiLanguageIntelligenceDiagnosticsPullState {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    UmiLanguageIntelligenceState state;
    uint32_t failure_count;
    uint32_t restart_count;
    uint64_t generation;
    uint64_t revision;
} UmiLanguageIntelligenceDiagnosticsPullState;
/**
 * Initialise language intelligence diagnostics pull state from caller-provided values so
 * later operations receive a known state.
 */
void umi_language_intelligence_diagnostics_pull_state_init(UmiLanguageIntelligenceDiagnosticsPullState *value, const char *id);
/**
 * Provide the language intelligence diagnostics pull state transition operation used by
 * this module and its client applications.
 */
UmiStatus umi_language_intelligence_diagnostics_pull_state_transition(
    UmiLanguageIntelligenceDiagnosticsPullState *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
/**
 * Provide the language intelligence diagnostics pull state record failure operation used
 * by this module and its client applications.
 */
void umi_language_intelligence_diagnostics_pull_state_record_failure(UmiLanguageIntelligenceDiagnosticsPullState *value);
/**
 * Provide the language intelligence diagnostics pull state is usable operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_diagnostics_pull_state_is_usable(const UmiLanguageIntelligenceDiagnosticsPullState *value);
#ifdef __cplusplus
}
#endif
#endif
