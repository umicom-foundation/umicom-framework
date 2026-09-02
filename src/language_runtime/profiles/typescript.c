/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/typescript.c
 *
 * PURPOSE:
 *   Define Framework-owned TypeScript Language Server executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/typescript.h"
/*
 * Provide the language runtime profile typescript operation used by this module and its
 * client applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_profile_typescript(void){static const UmiLanguageServerProfile p={.id="language.server.typescript",.display_name="TypeScript Language Server",.executable="typescript-language-server",.arguments="--stdio",.language_ids="javascript,typescript,javascriptreact,typescriptreact",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SIGNATURE | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_RENAME | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_CODE_ACTION | UMI_LANGUAGE_CAP_SEMANTIC_TOKENS | UMI_LANGUAGE_CAP_INLAY_HINTS | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
