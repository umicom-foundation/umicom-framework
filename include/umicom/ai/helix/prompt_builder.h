/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/prompt_builder.h
 *
 * PURPOSE:
 *   Build bounded role-specific system and operation prompts for Helix reasoning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_PROMPT_BUILDER_H
#define INCLUDE_UMICOM_AI_HELIX_PROMPT_BUILDER_H

#include <stddef.h>
#include "umicom/ai/helix/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_helix_prompt_build_system(UmiHelixAgentRole role,
                                           char *output,
                                           size_t output_capacity);
UmiStatus umi_ai_helix_prompt_build_user(const UmiAiHelixAgentRequest *request,
                                         char *output,
                                         size_t output_capacity);

#ifdef __cplusplus
}
#endif

#endif
