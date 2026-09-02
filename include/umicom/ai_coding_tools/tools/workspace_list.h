/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/workspace_list.h
 *
 * PURPOSE:
 *   List repository files using the Framework scanner with optional substring filtering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_WORKSPACE_LIST_H
#define UMICOM_AI_CODING_TOOL_WORKSPACE_LIST_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool workspace list descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_workspace_list_descriptor(void);
/**
 * Provide the ai coding tool workspace list invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_workspace_list_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
