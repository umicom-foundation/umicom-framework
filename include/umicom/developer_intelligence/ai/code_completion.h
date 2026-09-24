/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_intelligence/ai/code_completion.h
 *
 * PURPOSE:
 *   Evaluate AI Code Completion readiness and policy using bounded copied signals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_INTELLIGENCE_AI_CODE_COMPLETION_H
#define UMICOM_DEVELOPER_INTELLIGENCE_AI_CODE_COMPLETION_H

#include "umicom/developer_intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperIntelligenceEvaluateAiCodeCompletion(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
