/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/source_control_commit.h
 *
 * PURPOSE:
 *   Create a source-control commit using the existing Framework controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_SOURCE_CONTROL_COMMIT_H
#define UMICOM_AI_CODING_TOOL_SOURCE_CONTROL_COMMIT_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool source control commit descriptor operation used by this
 * module and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_commit_descriptor(void);
/**
 * Provide the ai coding tool source control commit invoke operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_coding_tool_source_control_commit_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
