/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/checkpoint_create.h
 *
 * PURPOSE:
 *   Capture complete-file workspace state for a bounded list of paths before a multi-step mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_CHECKPOINT_CREATE_H
#define UMICOM_AI_CODING_TOOL_CHECKPOINT_CREATE_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_checkpoint_create_descriptor(void);
UmiStatus umi_ai_coding_tool_checkpoint_create_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
