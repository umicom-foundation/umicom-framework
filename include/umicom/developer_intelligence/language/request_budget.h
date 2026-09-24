/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_intelligence/language/request_budget.h
 *
 * PURPOSE:
 *   Evaluate Language Request Budget readiness and policy using bounded copied signals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_INTELLIGENCE_LANGUAGE_REQUEST_BUDGET_H
#define UMICOM_DEVELOPER_INTELLIGENCE_LANGUAGE_REQUEST_BUDGET_H

#include "umicom/developer_intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageRequestBudget(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
