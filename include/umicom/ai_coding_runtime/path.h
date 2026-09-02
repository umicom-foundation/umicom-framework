/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/path.h
 *
 * PURPOSE:
 *   Provide bounded workspace-relative path validation and root confinement.
 *
 * SECURITY:
 *   Agent-generated paths are never treated as trusted. The helper rejects
 *   absolute paths, parent traversal and ambiguous separators before combining
 *   a path with a configured workspace root.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_PATH_H
#define UMICOM_AI_CODING_RUNTIME_PATH_H

#include "umicom/ai_coding_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding runtime path normalize relative operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_coding_runtime_path_normalize_relative(
    const char *path,
    char *out_path,
    size_t capacity);

/**
 * Provide the ai coding runtime path join operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_runtime_path_join(
    const char *root,
    const char *relative_path,
    char *out_path,
    size_t capacity);

/**
 * Provide the ai coding runtime path extension operation used by this module and its
 * client applications.
 */
const char *umi_ai_coding_runtime_path_extension(const char *path);

#ifdef __cplusplus
}
#endif
#endif
