/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/agent_continue_repair.h
 *
 * PURPOSE:
 *   Continue the bounded repair loop after validation failure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_AGENT_CONTINUE_REPAIR_H
#define UMICOM_AI_CODING_TOOL_AGENT_CONTINUE_REPAIR_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_agent_continue_repair_descriptor(void);
UmiStatus umi_ai_coding_tool_agent_continue_repair_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
