/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/ignore.h
 *
 * PURPOSE:
 *   Apply conservative default exclusions while indexing repository context.
 *   Build products, VCS internals, dependency caches and generated binaries are
 *   excluded before any content is read or sent to a model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_IGNORE_H
#define UMICOM_AI_CODING_RUNTIME_IGNORE_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding ignore policy data shared with callers of this public contract.
 */
typedef struct UmiAiCodingIgnorePolicy {
    int ignore_git;
    int ignore_build;
    int ignore_install;
    int ignore_dependencies;
    int ignore_hidden_directories;
    int ignore_binary_extensions;
} UmiAiCodingIgnorePolicy;

/**
 * Initialise ai coding ignore policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_ignore_policy_init(UmiAiCodingIgnorePolicy *policy);
/**
 * Provide the ai coding ignore path operation used by this module and its client
 * applications.
 */
int umi_ai_coding_ignore_path(
    const UmiAiCodingIgnorePolicy *policy,
    const char *relative_path,
    int directory);

#ifdef __cplusplus
}
#endif
#endif
