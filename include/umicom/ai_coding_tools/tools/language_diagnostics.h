/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/tools/language_diagnostics.h
 *
 * PURPOSE:
 *   List provider-neutral language diagnostics from the existing Framework language service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOL_LANGUAGE_DIAGNOSTICS_H
#define UMICOM_AI_CODING_TOOL_LANGUAGE_DIAGNOSTICS_H
#include "umicom/ai_coding_tools/tool_contract.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding tool language diagnostics descriptor operation used by this module
 * and its client applications.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_language_diagnostics_descriptor(void);
/**
 * Provide the ai coding tool language diagnostics invoke operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_tool_language_diagnostics_invoke(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data);
#ifdef __cplusplus
}
#endif
#endif
