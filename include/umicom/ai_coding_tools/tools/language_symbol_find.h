/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/language_symbol_find.h
 *
 * PURPOSE:
 *   Find one language symbol by stable Framework symbol identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_LANGUAGE_SYMBOL_FIND_H
#define UMICOM_AI_CODING_TOOL_LANGUAGE_SYMBOL_FIND_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool language symbol find descriptor operation used by this module
 * and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_language_symbol_find_descriptor(void);
/**
 * Provide the ai coding tool language symbol find invoke operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_language_symbol_find_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
