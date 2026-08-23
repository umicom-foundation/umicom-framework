/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/language.h
 *
 * PURPOSE:
 *   Detect practical editor/LSP language identifiers from repository paths for
 *   context indexing and prompt annotations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_LANGUAGE_H
#define UMICOM_AI_CODING_RUNTIME_LANGUAGE_H
#include "umicom/ai_coding_runtime/path.h"
#ifdef __cplusplus
extern "C" {
#endif
const char *umi_ai_coding_runtime_language_for_path(const char *path);
int umi_ai_coding_runtime_path_is_text_source(const char *path);
#ifdef __cplusplus
}
#endif
#endif
