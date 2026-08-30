/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/clangd.c
 *
 * PURPOSE:
 *   Define Framework-owned Clangd executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/clangd.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_clangd(void){static const UmiLanguageServerProfile p={.id="language.server.clangd",.display_name="Clangd",.executable="clangd",.arguments="--background-index --clang-tidy --completion-style=detailed",.language_ids="c,cpp,assembly",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SIGNATURE | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_RENAME | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_CODE_ACTION | UMI_LANGUAGE_CAP_SEMANTIC_TOKENS | UMI_LANGUAGE_CAP_INLAY_HINTS | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
