/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/types.c
 *
 * PURPOSE:
 *   Translate shared AI enumerations into stable diagnostic text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Keeping these conversions in one place makes logs and command-line tools consistent across providers.
 */

#include "umicom/ai/types.h"

/* Provide the ai role text operation used by this module and its client applications. */
const char *umi_ai_role_text(UmiAiRole role)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (role) {
        case UMI_AI_ROLE_SYSTEM: return "system";
        case UMI_AI_ROLE_USER: return "user";
        case UMI_AI_ROLE_ASSISTANT: return "assistant";
        case UMI_AI_ROLE_TOOL: return "tool";
        default: return "unknown";
    }
}

/*
 * Provide the ai finish reason text operation used by this module and its client
 * applications.
 */
const char *umi_ai_finish_reason_text(UmiAiFinishReason reason)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (reason) {
        case UMI_AI_FINISH_NONE: return "none";
        case UMI_AI_FINISH_STOP: return "stop";
        case UMI_AI_FINISH_LENGTH: return "length";
        case UMI_AI_FINISH_CANCELLED: return "cancelled";
        case UMI_AI_FINISH_ERROR: return "error";
        default: return "unknown";
    }
}

/*
 * Provide the ai provider kind text operation used by this module and its client
 * applications.
 */
const char *umi_ai_provider_kind_text(UmiAiProviderKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_AI_PROVIDER_LOCAL: return "local";
        case UMI_AI_PROVIDER_REMOTE: return "remote";
        case UMI_AI_PROVIDER_AUTHOR_ENGINE: return "authorengine";
        case UMI_AI_PROVIDER_TEST: return "test";
        default: return "unknown";
    }
}
