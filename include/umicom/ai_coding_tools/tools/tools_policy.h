/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/tools_policy.h
 *
 * PURPOSE:
 *   Read the active central coding-tool policy without exposing private service pointers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_TOOLS_POLICY_H
#define UMICOM_AI_CODING_TOOL_TOOLS_POLICY_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool tools policy descriptor operation used by this module and its
 * client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_tools_policy_descriptor(void);
/**
 * Provide the ai coding tool tools policy invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_tools_policy_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
