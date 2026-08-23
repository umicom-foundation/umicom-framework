/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/workspace_search.h
 *
 * PURPOSE:
 *   Search repository text files for a literal query and return bounded path/line matches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_WORKSPACE_SEARCH_H
#define UMICOM_AI_CODING_TOOL_WORKSPACE_SEARCH_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_workspace_search_descriptor(void);
UmiStatus umi_ai_coding_tool_workspace_search_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
