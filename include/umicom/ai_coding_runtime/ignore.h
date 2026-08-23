/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/ignore.h
 *
 * PURPOSE:
 *   Apply conservative default exclusions while indexing repository context.
 *   Build products, VCS internals, dependency caches and generated binaries are
 *   excluded before any content is read or sent to a model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_IGNORE_H
#define UMICOM_AI_CODING_RUNTIME_IGNORE_H
#include "umicom/ai_coding_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingIgnorePolicy {
    int ignore_git;
    int ignore_build;
    int ignore_install;
    int ignore_dependencies;
    int ignore_hidden_directories;
    int ignore_binary_extensions;
} UmiAiCodingIgnorePolicy;

void umi_ai_coding_ignore_policy_init(UmiAiCodingIgnorePolicy *policy);
int umi_ai_coding_ignore_path(
    const UmiAiCodingIgnorePolicy *policy,
    const char *relative_path,
    int directory);

#ifdef __cplusplus
}
#endif
#endif
