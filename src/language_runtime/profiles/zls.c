/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/zls.c
 *
 * PURPOSE:
 *   Define Framework-owned ZLS executable and capability metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/zls.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_zls(void){static const UmiLanguageServerProfile p={.id="language.server.zls",.display_name="ZLS",.executable="zls",.arguments="",.language_ids="zig",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SIGNATURE | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_RENAME | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_CODE_ACTION | UMI_LANGUAGE_CAP_SEMANTIC_TOKENS | UMI_LANGUAGE_CAP_INLAY_HINTS | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
