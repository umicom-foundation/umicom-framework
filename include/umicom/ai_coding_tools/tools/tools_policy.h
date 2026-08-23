/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/tools_policy.h
 *
 * PURPOSE:
 *   Read the active central coding-tool policy without exposing private service pointers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_TOOLS_POLICY_H
#define UMICOM_AI_CODING_TOOL_TOOLS_POLICY_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_tools_policy_descriptor(void);
UmiStatus umi_ai_coding_tool_tools_policy_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
