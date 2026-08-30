/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/response.h
 *
 * PURPOSE:
 *   Represent a parsed model response and optional governed complete-file patch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_RESPONSE_H
#define UMICOM_AI_CODING_RUNTIME_RESPONSE_H

#include "umicom/ai_coding_runtime/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingParsedResponse {
    char summary[UMI_AI_CODING_RUNTIME_TEXT_CAPACITY];
    UmiAiCodingPatch patch;
    int structured;
    int has_patch;
    uint64_t revision;
} UmiAiCodingParsedResponse;

UmiStatus umi_ai_coding_parse_response(
    const UmiAiResponse *response,
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingParsedResponse *out_response);

#ifdef __cplusplus
}
#endif
#endif
