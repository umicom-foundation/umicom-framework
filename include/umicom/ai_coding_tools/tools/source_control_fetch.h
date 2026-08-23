/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/source_control_fetch.h
 *
 * PURPOSE:
 *   Fetch Source Control through the existing provider-neutral controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_SOURCE_CONTROL_FETCH_H
#define UMICOM_AI_CODING_TOOL_SOURCE_CONTROL_FETCH_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_source_control_fetch_descriptor(void);
UmiStatus umi_ai_coding_tool_source_control_fetch_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
