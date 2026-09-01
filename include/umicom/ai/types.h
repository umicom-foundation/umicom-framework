/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/types.h
 *
 * PURPOSE:
 *   Define shared AI roles, finish reasons, provider kinds, capacities and usage counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * These basic enums and capacities are reused by every AI provider so Studio and future Umicom products speak one stable vocabulary.
 */

#ifndef INCLUDE_UMICOM_AI_TYPES_H
#define INCLUDE_UMICOM_AI_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_ID_CAPACITY 128U
#define UMI_AI_TEXT_CAPACITY 2048U
#define UMI_AI_SMALL_TEXT_CAPACITY 512U
#define UMI_AI_MAX_PROVIDERS 16U
#define UMI_AI_MAX_TOOLS 64U
#define UMI_AI_MAX_MESSAGES 64U
#define UMI_AI_MAX_RETRIEVAL_RESULTS 16U
#define UMI_AI_EMBEDDING_CAPACITY 64U

typedef enum UmiAiRole {
    UMI_AI_ROLE_SYSTEM = 1,
    UMI_AI_ROLE_USER = 2,
    UMI_AI_ROLE_ASSISTANT = 3,
    UMI_AI_ROLE_TOOL = 4
} UmiAiRole;

typedef enum UmiAiFinishReason {
    UMI_AI_FINISH_NONE = 0,
    UMI_AI_FINISH_STOP = 1,
    UMI_AI_FINISH_LENGTH = 2,
    UMI_AI_FINISH_CANCELLED = 3,
    UMI_AI_FINISH_ERROR = 4
} UmiAiFinishReason;

typedef enum UmiAiProviderKind {
    UMI_AI_PROVIDER_LOCAL = 1,
    UMI_AI_PROVIDER_REMOTE = 2,
    UMI_AI_PROVIDER_AUTHOR_ENGINE = 3,
    UMI_AI_PROVIDER_TEST = 4
} UmiAiProviderKind;

typedef struct UmiAiUsage {
    uint64_t input_tokens;
    uint64_t output_tokens;
    uint64_t total_tokens;
} UmiAiUsage;

const char *umi_ai_role_text(UmiAiRole role);
const char *umi_ai_finish_reason_text(UmiAiFinishReason reason);
const char *umi_ai_provider_kind_text(UmiAiProviderKind kind);

#ifdef __cplusplus
}
#endif

#endif
