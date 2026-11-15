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
/* Admit canonical selected-file metadata and bound all counts before reads.
 * The plan and callback table are copied for this synchronous call. User data
 * stays borrowed; all inputs and outputs require live, serialized ownership.
 * On failure, out_context is unchanged unless the callback itself writes it,
 * which is forbidden. Successful reads supply length < text capacity and no
 * embedded NUL; Framework owns the trailing terminator. Explicit NOT_FOUND,
 * CAPACITY_EXCEEDED and PARSE_ERROR callbacks retain their skip behavior.
 * An OK callback with an impossible length fails the entire materialization.
 * This validates data shape, not filesystem containment or renewed consent. */
UmiStatus umi_ai_coding_materialize_context(
    const UmiAiCodingTaskPlan *plan,
    const UmiAiCodingWorkspaceAdapter *workspace,
    UmiAiCodingMaterializedContext *out_context);

/* Validate the selected metadata and active path consumed by materialization.
 * Does not validate unused request fields or establish consent. Consumers that
 * use the complete coding request must also validate that request. */
UmiStatus UmiAiCodingMaterializationPlanValidate(const UmiAiCodingTaskPlan *plan);

/* Validate bounded paths/text, lengths, content hashes, unique paths and token
 * totals. Empty contexts are valid. NULL is INVALID_ARGUMENT; malformed values
 * return INVALID_STATE. Revision is carried provenance, not authorization. */
UmiStatus UmiAiCodingMaterializedContextValidate(const UmiAiCodingMaterializedContext *context);

/**
 * Find ai coding materialized context while leaving the underlying catalogue or model
 * owned by this module.
 */
/* Validate the complete context before lookup. path must terminate within
 * UMI_AI_CODING_RUNTIME_PATH_CAPACITY bytes and be lexically relative. Output
 * is preserved on refusal or a missing path; self-copy of a found file is valid. */
UmiStatus umi_ai_coding_materialized_context_find(
    const UmiAiCodingMaterializedContext *context,
    const char *path,
    UmiAiCodingMaterializedFile *out_file);

#ifdef __cplusplus
}
#endif
#endif
