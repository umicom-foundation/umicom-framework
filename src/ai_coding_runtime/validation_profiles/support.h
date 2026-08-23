/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_profiles/support.h
 *
 * PURPOSE:
 *   Internal helper for concise, consistent validation step construction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_VALIDATION_PROFILE_SUPPORT_H
#define UMICOM_AI_CODING_RUNTIME_VALIDATION_PROFILE_SUPPORT_H
#include "umicom/ai_coding_runtime/validation_plan.h"
UmiStatus umi_ai_coding_validation_profile_add(
    UmiAiCodingValidationPlan *plan,
    const char *id,
    UmiAiCodingValidationKind kind,
    const char *label,
    const char *program,
    const char *arguments,
    const char *working_directory,
    int required,
    uint32_t timeout_ms);
#endif
