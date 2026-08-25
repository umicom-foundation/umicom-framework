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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_PULL_STATE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_PULL_STATE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_DIAGNOSTICS_PULL_STATE_API_VERSION 1U
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
void umi_language_intelligence_diagnostics_pull_state_init(UmiLanguageIntelligenceDiagnosticsPullState *value, const char *id);
UmiStatus umi_language_intelligence_diagnostics_pull_state_transition(
    UmiLanguageIntelligenceDiagnosticsPullState *value,
    UmiLanguageIntelligenceState next_state,
    int explicit_release);
void umi_language_intelligence_diagnostics_pull_state_record_failure(UmiLanguageIntelligenceDiagnosticsPullState *value);
int umi_language_intelligence_diagnostics_pull_state_is_usable(const UmiLanguageIntelligenceDiagnosticsPullState *value);
#ifdef __cplusplus
}
#endif
#endif
