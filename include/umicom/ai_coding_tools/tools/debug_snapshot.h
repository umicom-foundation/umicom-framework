/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/debug_snapshot.h
 *
 * PURPOSE:
 *   Read the current Framework DAP runtime state without controlling the debuggee.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_DEBUG_SNAPSHOT_H
#define UMICOM_AI_CODING_TOOL_DEBUG_SNAPSHOT_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_debug_snapshot_descriptor(void);
UmiStatus umi_ai_coding_tool_debug_snapshot_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
