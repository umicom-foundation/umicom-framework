/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/developer_package.h
 *
 * PURPOSE:
 *   Execute a project packaging command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_DEVELOPER_PACKAGE_H
#define UMICOM_AI_CODING_TOOL_DEVELOPER_PACKAGE_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool developer package descriptor operation used by this module
 * and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_developer_package_descriptor(void);
/**
 * Provide the ai coding tool developer package invoke operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_developer_package_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
