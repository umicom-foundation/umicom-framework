/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/language_symbols.h
 *
 * PURPOSE:
 *   List provider-neutral language symbols from the existing Framework language service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_LANGUAGE_SYMBOLS_H
#define UMICOM_AI_CODING_TOOL_LANGUAGE_SYMBOLS_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_language_symbols_descriptor(void);
UmiStatus umi_ai_coding_tool_language_symbols_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
