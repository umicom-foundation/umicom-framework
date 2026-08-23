/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/pyright.c
 *
 * PURPOSE:
 *   Define Framework-owned Pyright executable and capability metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/pyright.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_pyright(void){static const UmiLanguageServerProfile p={.id="language.server.pyright",.display_name="Pyright",.executable="pyright-langserver",.arguments="--stdio",.language_ids="python",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SIGNATURE | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_RENAME | UMI_LANGUAGE_CAP_CODE_ACTION | UMI_LANGUAGE_CAP_SEMANTIC_TOKENS | UMI_LANGUAGE_CAP_INLAY_HINTS | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
