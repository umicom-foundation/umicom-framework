/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/agent_reject.h
 *
 * PURPOSE:
 *   Reject the pending governed coding patch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_AGENT_REJECT_H
#define UMICOM_AI_CODING_TOOL_AGENT_REJECT_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool agent reject descriptor operation used by this module and its
 * client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_agent_reject_descriptor(void);
/**
 * Provide the ai coding tool agent reject invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_agent_reject_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
