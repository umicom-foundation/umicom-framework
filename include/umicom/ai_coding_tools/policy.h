/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/policy.h
 *
 * PURPOSE:
 *   Centralize capability, risk, approval and network policy for model-requested
 *   developer tool calls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_POLICY_H
#define UMICOM_AI_CODING_TOOLS_POLICY_H
#include "umicom/ai_coding_tools/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding tool policy data shared with callers of this public contract.
 */
typedef struct UmiAiCodingToolPolicy {
    uint64_t allowed_capabilities;
    uint64_t auto_approved_capabilities;
    size_t maximum_plan_steps;
    size_t maximum_output_bytes;
    int allow_network;
    int allow_process_execution;
    int allow_source_control_mutation;
    int allow_debug_control;
    int allow_checkpoint_restore;
} UmiAiCodingToolPolicy;

/**
 * Initialise ai coding tool policy from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_coding_tool_policy_init(UmiAiCodingToolPolicy *policy);

/**
 * Provide the ai coding tool policy check operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_policy_check(
    const UmiAiCodingToolPolicy *policy,
    const UmiAiCodingToolDescriptor *descriptor,
    int approved,
    int *out_approval_required);

#ifdef __cplusplus
}
#endif
#endif
