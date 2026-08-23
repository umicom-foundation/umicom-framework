/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/workspace_exists.h
 *
 * PURPOSE:
 *   Check whether a root-confined workspace path exists.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_WORKSPACE_EXISTS_H
#define UMICOM_AI_CODING_TOOL_WORKSPACE_EXISTS_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_workspace_exists_descriptor(void);
UmiStatus umi_ai_coding_tool_workspace_exists_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
