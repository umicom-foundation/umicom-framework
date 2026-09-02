/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/checkpoint_list.h
 *
 * PURPOSE:
 *   List in-memory workspace checkpoints and their protected files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_CHECKPOINT_LIST_H
#define UMICOM_AI_CODING_TOOL_CHECKPOINT_LIST_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool checkpoint list descriptor operation used by this module and
 * its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_checkpoint_list_descriptor(void);
/**
 * Provide the ai coding tool checkpoint list invoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_checkpoint_list_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
