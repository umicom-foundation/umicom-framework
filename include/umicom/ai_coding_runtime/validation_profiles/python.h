/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/validation_profiles/python.h
 *
 * PURPOSE:
 *   Build the reusable python coding-agent validation plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_VALIDATION_PROFILE_PYTHON_H
#define UMICOM_AI_CODING_RUNTIME_VALIDATION_PROFILE_PYTHON_H
#include "umicom/ai_coding_runtime/validation_plan.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding validation profile python operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_validation_profile_python(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan);
#ifdef __cplusplus
}
#endif
#endif
