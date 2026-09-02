/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/rust_analyzer.c
 *
 * PURPOSE:
 *   Define Framework-owned rust-analyzer executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/rust_analyzer.h"
/*
 * Provide the language runtime profile rust analyzer operation used by this module and its
 * client applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_profile_rust_analyzer(void){static const UmiLanguageServerProfile p={.id="language.server.rust-analyzer",.display_name="rust-analyzer",.executable="rust-analyzer",.arguments="",.language_ids="rust",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SIGNATURE | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_RENAME | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_CODE_ACTION | UMI_LANGUAGE_CAP_SEMANTIC_TOKENS | UMI_LANGUAGE_CAP_INLAY_HINTS | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
