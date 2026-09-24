/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_intelligence/language/signature_active_parameter.h
 *
 * PURPOSE:
 *   Evaluate Signature Active Parameter readiness and policy using bounded copied signals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_INTELLIGENCE_LANGUAGE_SIGNATURE_ACTIVE_PARAMETER_H
#define UMICOM_DEVELOPER_INTELLIGENCE_LANGUAGE_SIGNATURE_ACTIVE_PARAMETER_H

#include "umicom/developer_intelligence/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperIntelligenceEvaluateLanguageSignatureActiveParameter(
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
