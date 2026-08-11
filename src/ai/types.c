/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/types.c
 *
 * PURPOSE:
 *   Translate shared AI enumerations into stable diagnostic text.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Keeping these conversions in one place makes logs and command-line tools consistent across providers.
 */

#include "umicom/ai/types.h"

const char *umi_ai_role_text(UmiAiRole role)
{
    switch (role) {
        case UMI_AI_ROLE_SYSTEM: return "system";
        case UMI_AI_ROLE_USER: return "user";
        case UMI_AI_ROLE_ASSISTANT: return "assistant";
        case UMI_AI_ROLE_TOOL: return "tool";
        default: return "unknown";
    }
}

const char *umi_ai_finish_reason_text(UmiAiFinishReason reason)
{
    switch (reason) {
        case UMI_AI_FINISH_NONE: return "none";
        case UMI_AI_FINISH_STOP: return "stop";
        case UMI_AI_FINISH_LENGTH: return "length";
        case UMI_AI_FINISH_CANCELLED: return "cancelled";
        case UMI_AI_FINISH_ERROR: return "error";
        default: return "unknown";
    }
}

const char *umi_ai_provider_kind_text(UmiAiProviderKind kind)
{
    switch (kind) {
        case UMI_AI_PROVIDER_LOCAL: return "local";
        case UMI_AI_PROVIDER_REMOTE: return "remote";
        case UMI_AI_PROVIDER_AUTHOR_ENGINE: return "authorengine";
        case UMI_AI_PROVIDER_TEST: return "test";
        default: return "unknown";
    }
}
