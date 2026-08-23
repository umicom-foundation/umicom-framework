/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/json.c
 *
 * PURPOSE:
 *   Define Framework-owned JSON Language Server executable and capability metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/json.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_json(void){static const UmiLanguageServerProfile p={.id="language.server.json",.display_name="JSON Language Server",.executable="vscode-json-language-server",.arguments="--stdio",.language_ids="json,jsonc",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_CODE_ACTION | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
