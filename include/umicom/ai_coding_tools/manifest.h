/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/manifest.h
 *
 * PURPOSE:
 *   Publish concise JSON argument contracts for built-in coding tools so a
 *   provider can reason about available operations without embedding schemas in
 *   Studio or another product.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_MANIFEST_H
#define UMICOM_AI_CODING_TOOLS_MANIFEST_H
#include "umicom/ai_coding_tools/catalogue.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingToolManifestEntry {
    char tool_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    char arguments_schema[1024];
    char result_summary[512];
} UmiAiCodingToolManifestEntry;

size_t umi_ai_coding_tool_manifest_count(void);
UmiStatus umi_ai_coding_tool_manifest_at(
    size_t index,
    UmiAiCodingToolManifestEntry *out_entry);
UmiStatus umi_ai_coding_tool_manifest_find(
    const char *tool_id,
    UmiAiCodingToolManifestEntry *out_entry);

#ifdef __cplusplus
}
#endif
#endif
