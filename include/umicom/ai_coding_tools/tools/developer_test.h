/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/developer_test.h
 *
 * PURPOSE:
 *   Execute a project test command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_DEVELOPER_TEST_H
#define UMICOM_AI_CODING_TOOL_DEVELOPER_TEST_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool developer test descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_developer_test_descriptor(void);
/**
 * Provide the ai coding tool developer test invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_developer_test_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
