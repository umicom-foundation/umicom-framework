/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/validation_profiles/zig.h
 *
 * PURPOSE:
 *   Build the reusable zig coding-agent validation plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_VALIDATION_PROFILE_ZIG_H
#define UMICOM_AI_CODING_RUNTIME_VALIDATION_PROFILE_ZIG_H
#include "umicom/ai_coding_runtime/validation_plan.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding validation profile zig operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_validation_profile_zig(
    const char *workspace_root,
    UmiAiCodingValidationPlan *out_plan);
#ifdef __cplusplus
}
#endif
#endif
