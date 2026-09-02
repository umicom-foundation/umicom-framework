/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/catalogue.h
 *
 * PURPOSE:
 *   Enumerate Framework-owned coding tools and resolve their richer policy
 *   descriptors independently of the existing generic UmiAiTool registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_CATALOGUE_H
#define UMICOM_AI_CODING_TOOLS_CATALOGUE_H
#include "umicom/ai_coding_tools/environment.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return the number of records represented by ai coding tool catalogue without changing
 * their state.
 */
size_t umi_ai_coding_tool_catalogue_count(void);
/**
 * Find ai coding tool catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_catalogue_at(size_t index);
/**
 * Find ai coding tool catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiCodingToolDescriptor *umi_ai_coding_tool_catalogue_find(
    const char *tool_id);

/**
 * Provide the ai coding tool register all operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_register_all(
    UmiAiCodingToolEnvironment *environment);

#ifdef __cplusplus
}
#endif
#endif
