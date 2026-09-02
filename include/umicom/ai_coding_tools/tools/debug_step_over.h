/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/debug_step_over.h
 *
 * PURPOSE:
 *   Step Over through the real Framework DAP runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_DEBUG_STEP_OVER_H
#define UMICOM_AI_CODING_TOOL_DEBUG_STEP_OVER_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool debug step over descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_step_over_descriptor(void);
/**
 * Provide the ai coding tool debug step over invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_debug_step_over_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
