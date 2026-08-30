/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/checkpoint_restore.h
 *
 * PURPOSE:
 *   Restore a previously captured workspace checkpoint through the controlled adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_CHECKPOINT_RESTORE_H
#define UMICOM_AI_CODING_TOOL_CHECKPOINT_RESTORE_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_checkpoint_restore_descriptor(void);
UmiStatus umi_ai_coding_tool_checkpoint_restore_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
