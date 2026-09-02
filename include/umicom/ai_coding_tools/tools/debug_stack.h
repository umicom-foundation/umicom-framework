/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/debug_stack.h
 *
 * PURPOSE:
 *   Refresh the stack trace for a specified DAP thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_DEBUG_STACK_H
#define UMICOM_AI_CODING_TOOL_DEBUG_STACK_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool debug stack descriptor operation used by this module and its
 * client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_stack_descriptor(void);
/**
 * Provide the ai coding tool debug stack invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_debug_stack_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
