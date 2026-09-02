/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/source_control_fetch.h
 *
 * PURPOSE:
 *   Fetch Source Control through the existing provider-neutral controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_SOURCE_CONTROL_FETCH_H
#define UMICOM_AI_CODING_TOOL_SOURCE_CONTROL_FETCH_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool source control fetch descriptor operation used by this module
 * and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_fetch_descriptor(void);
/**
 * Provide the ai coding tool source control fetch invoke operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_source_control_fetch_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
