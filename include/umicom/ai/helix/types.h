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

/**
 * List the named ai helix disposition values accepted by this public contract.
 */
typedef enum UmiAiHelixDisposition {
    UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION = 1,
    UMI_AI_HELIX_DISPOSITION_COMPLETE = 2,
    UMI_AI_HELIX_DISPOSITION_NEEDS_CONTEXT = 3,
    UMI_AI_HELIX_DISPOSITION_DEFER = 4
} UmiAiHelixDisposition;

/**
 * Represent the ai helix agent request data shared with callers of this public contract.
 */
typedef struct UmiAiHelixAgentRequest {
    UmiHelixAgentRole role;
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char objective[UMI_HELIX_TEXT_CAPACITY];
    char context[UMI_AI_HELIX_CONTEXT_CAPACITY];
    char target_hint[UMI_HELIX_TEXT_CAPACITY];
    int provider_approved;
} UmiAiHelixAgentRequest;

/**
 * Represent the ai helix agent result data shared with callers of this public contract.
 */
typedef struct UmiAiHelixAgentResult {
    UmiAiHelixDisposition disposition;
    UmiHelixAgentRole role;
    UmiHelixAction action;
    char rationale[UMI_AI_TEXT_CAPACITY];
    char raw_response[UMI_AI_HELIX_RAW_RESPONSE_CAPACITY];
    UmiAiUsage usage;
    double confidence;
} UmiAiHelixAgentResult;

/**
 * Initialise ai helix agent request from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_helix_agent_request_init(UmiAiHelixAgentRequest *request);
/**
 * Initialise ai helix agent result from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_helix_agent_result_init(UmiAiHelixAgentResult *result);
/**
 * Provide the ai helix disposition text operation used by this module and its client
 * applications.
 */
const char *umi_ai_helix_disposition_text(UmiAiHelixDisposition disposition);
/**
 * Provide the ai helix action mask operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_helix_action_mask(UmiHelixActionKind kind);

#ifdef __cplusplus
}
#endif

#endif
