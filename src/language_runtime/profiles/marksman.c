/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/marksman.c
 *
 * PURPOSE:
 *   Define Framework-owned Marksman executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/marksman.h"
/*
 * Provide the language runtime profile marksman operation used by this module and its
 * client applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_profile_marksman(void){static const UmiLanguageServerProfile p={.id="language.server.marksman",.display_name="Marksman",.executable="marksman",.arguments="server",.language_ids="markdown",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_RENAME | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
