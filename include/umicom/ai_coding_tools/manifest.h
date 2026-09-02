/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/manifest.h
 *
 * PURPOSE:
 *   Publish concise JSON argument contracts for built-in coding tools so a
 *   provider can reason about available operations without embedding schemas in
 *   Studio or another product.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_MANIFEST_H
#define UMICOM_AI_CODING_TOOLS_MANIFEST_H
#include "umicom/ai_coding_tools/catalogue.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding tool manifest entry data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingToolManifestEntry {
    char tool_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    char arguments_schema[1024];
    char result_summary[512];
} UmiAiCodingToolManifestEntry;

/**
 * Return the number of records represented by ai coding tool manifest without changing
 * their state.
 */
size_t umi_ai_coding_tool_manifest_count(void);
/**
 * Find ai coding tool manifest while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_coding_tool_manifest_at(
    size_t index,
    UmiAiCodingToolManifestEntry *out_entry);
/**
 * Find ai coding tool manifest while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_coding_tool_manifest_find(
    const char *tool_id,
    UmiAiCodingToolManifestEntry *out_entry);

#ifdef __cplusplus
}
#endif
#endif
