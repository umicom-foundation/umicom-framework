/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/call_parser.h
 *
 * PURPOSE:
 *   Parse a bounded textual tool-call protocol emitted by providers that do not
 *   expose native structured tool calling.
 *
 * PROTOCOL:
 *   UMICOM-TOOL-CALL/1
 *   TOOL|workspace.read
 *   APPROVED|0
 *   ARGUMENTS-BEGIN
 *   {"path":"src/main.c"}
 *   ARGUMENTS-END
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_CALL_PARSER_H
#define UMICOM_AI_CODING_TOOLS_CALL_PARSER_H
#include "umicom/ai_coding_tools/types.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_coding_tool_call_parse(
    const char *text,
    uint64_t call_id,
    UmiAiCodingToolCall *out_call);

#ifdef __cplusplus
}
#endif
#endif
