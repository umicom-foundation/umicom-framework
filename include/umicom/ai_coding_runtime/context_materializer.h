/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/context_materializer.h
 *
 * PURPOSE:
 *   Materialize only the repository files selected by the existing privacy and
 *   token-budget context planner. Metadata planning remains separate from file
 *   reads so no unapproved repository content crosses an AI boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_CONTEXT_MATERIALIZER_H
#define UMICOM_AI_CODING_RUNTIME_CONTEXT_MATERIALIZER_H

#include "umicom/ai_coding_runtime/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding materialize context operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_materialize_context(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingMaterializedContext *out_context);

/**
 * Find ai coding materialized context while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_ai_coding_materialized_context_find(
    const UmiAiCodingMaterializedContext *context,
    const char *path,
    UmiAiCodingMaterializedFile *out_file);

#ifdef __cplusplus
}
#endif
#endif
