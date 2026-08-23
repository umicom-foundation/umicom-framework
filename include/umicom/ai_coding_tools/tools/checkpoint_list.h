/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/checkpoint_list.h
 *
 * PURPOSE:
 *   List in-memory workspace checkpoints and their protected files.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_CHECKPOINT_LIST_H
#define UMICOM_AI_CODING_TOOL_CHECKPOINT_LIST_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_checkpoint_list_descriptor(void);
UmiStatus umi_ai_coding_tool_checkpoint_list_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
