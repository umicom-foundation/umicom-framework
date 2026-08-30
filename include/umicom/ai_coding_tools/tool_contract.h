/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tool_contract.h
 *
 * PURPOSE:
 *   Define the uniform declaration contract implemented by every built-in
 *   coding tool.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_TOOL_CONTRACT_H
#define UMICOM_AI_CODING_TOOLS_TOOL_CONTRACT_H
#include "umicom/ai_coding_tools/environment.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef const UmiAiCodingToolDescriptor *(*UmiAiCodingToolDescriptorFactory)(void);
typedef UmiStatus (*UmiAiCodingBuiltinToolInvoke)(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
