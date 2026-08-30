/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/html.c
 *
 * PURPOSE:
 *   Define Framework-owned HTML Language Server executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/html.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_html(void){static const UmiLanguageServerProfile p={.id="language.server.html",.display_name="HTML Language Server",.executable="vscode-html-language-server",.arguments="--stdio",.language_ids="html",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
