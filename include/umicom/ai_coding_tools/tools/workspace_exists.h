/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/workspace_exists.h
 *
 * PURPOSE:
 *   Check whether a root-confined workspace path exists.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_WORKSPACE_EXISTS_H
#define UMICOM_AI_CODING_TOOL_WORKSPACE_EXISTS_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool workspace exists descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_workspace_exists_descriptor(void);
/**
 * Provide the ai coding tool workspace exists invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_workspace_exists_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
