/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/source_control_commit.h
 *
 * PURPOSE:
 *   Create a source-control commit using the existing Framework controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_SOURCE_CONTROL_COMMIT_H
#define UMICOM_AI_CODING_TOOL_SOURCE_CONTROL_COMMIT_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_commit_descriptor(void);
UmiStatus umi_ai_coding_tool_source_control_commit_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
