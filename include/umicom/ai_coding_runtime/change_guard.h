/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/change_guard.h
 *
 * PURPOSE:
 *   Apply an additional agent-safety gate before a model-generated patch enters
 *   approval. The guard can prohibit deletion, generated/build paths and
 *   unexpectedly broad changes independently of model instructions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_CHANGE_GUARD_H
#define UMICOM_AI_CODING_RUNTIME_CHANGE_GUARD_H
#include "umicom/ai_coding_runtime/patch_preview.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding change guard policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingChangeGuardPolicy {
    size_t maximum_files;
    uint32_t maximum_changed_lines;
    int allow_delete;
    int allow_generated_paths;
    int allow_dependency_paths;
} UmiAiCodingChangeGuardPolicy;

/**
 * Represent the ai coding change guard result data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingChangeGuardResult {
    int accepted;
    size_t rejected_files;
    char reason[1024];
} UmiAiCodingChangeGuardResult;

/**
 * Initialise ai coding change guard policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_change_guard_policy_init(
    UmiAiCodingChangeGuardPolicy *policy);

/**
 * Provide the ai coding change guard check operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_change_guard_check(
    const UmiAiCodingPatch *patch,
    const UmiAiCodingChangeGuardPolicy *policy,
    UmiAiCodingChangeGuardResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
