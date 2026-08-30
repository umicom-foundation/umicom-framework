/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/types.h
 *
 * PURPOSE:
 *   Define bounded provider-neutral AI/Helix bridge request and result types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_TYPES_H
#define INCLUDE_UMICOM_AI_HELIX_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"
#include "umicom/helix/action.h"
#include "umicom/helix/execution_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_HELIX_CONTEXT_CAPACITY 768U
#define UMI_AI_HELIX_RAW_RESPONSE_CAPACITY (UMI_AI_TEXT_CAPACITY * 2U)

typedef enum UmiAiHelixDisposition {
    UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION = 1,
    UMI_AI_HELIX_DISPOSITION_COMPLETE = 2,
    UMI_AI_HELIX_DISPOSITION_NEEDS_CONTEXT = 3,
    UMI_AI_HELIX_DISPOSITION_DEFER = 4
} UmiAiHelixDisposition;

typedef struct UmiAiHelixAgentRequest {
    UmiHelixAgentRole role;
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char objective[UMI_HELIX_TEXT_CAPACITY];
    char context[UMI_AI_HELIX_CONTEXT_CAPACITY];
    char target_hint[UMI_HELIX_TEXT_CAPACITY];
    int provider_approved;
} UmiAiHelixAgentRequest;

typedef struct UmiAiHelixAgentResult {
    UmiAiHelixDisposition disposition;
    UmiHelixAgentRole role;
    UmiHelixAction action;
    char rationale[UMI_AI_TEXT_CAPACITY];
    char raw_response[UMI_AI_HELIX_RAW_RESPONSE_CAPACITY];
    UmiAiUsage usage;
    double confidence;
} UmiAiHelixAgentResult;

void umi_ai_helix_agent_request_init(UmiAiHelixAgentRequest *request);
void umi_ai_helix_agent_result_init(UmiAiHelixAgentResult *result);
const char *umi_ai_helix_disposition_text(UmiAiHelixDisposition disposition);
uint32_t umi_ai_helix_action_mask(UmiHelixActionKind kind);

#ifdef __cplusplus
}
#endif

#endif
