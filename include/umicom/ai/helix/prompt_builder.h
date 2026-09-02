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

/**
 * Provide the ai helix prompt build system operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_helix_prompt_build_system(UmiHelixAgentRole role,
                                           char *output,
                                           size_t output_capacity);
/**
 * Provide the ai helix prompt build user operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_helix_prompt_build_user(const UmiAiHelixAgentRequest *request,
                                         char *output,
                                         size_t output_capacity);

#ifdef __cplusplus
}
#endif

#endif
