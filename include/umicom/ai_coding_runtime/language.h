/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/language.h
 *
 * PURPOSE:
 *   Detect practical editor/LSP language identifiers from repository paths for
 *   context indexing and prompt annotations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_LANGUAGE_H
#define UMICOM_AI_CODING_RUNTIME_LANGUAGE_H
#include "umicom/ai_coding_runtime/path.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ai coding runtime language for path operation used by this module and its
 * client applications.
 */
const char *umi_ai_coding_runtime_language_for_path(const char *path);
/**
 * Provide the ai coding runtime path is text source operation used by this module and its
 * client applications.
 */
int umi_ai_coding_runtime_path_is_text_source(const char *path);
#ifdef __cplusplus
}
#endif
#endif
