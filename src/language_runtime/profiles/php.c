/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/php.c
 *
 * PURPOSE:
 *   Define Framework-owned Intelephense executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/php.h"
/*
 * Provide the language runtime profile php operation used by this module and its client
 * applications.
 */
const UmiLanguageServerProfile *umi_language_runtime_profile_php(void){static const UmiLanguageServerProfile p={.id="language.server.php",.display_name="Intelephense",.executable="intelephense",.arguments="--stdio",.language_ids="php",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SIGNATURE | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_RENAME | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_CODE_ACTION,.enabled=1,.autostart=1,.revision=1U};return &p;}
