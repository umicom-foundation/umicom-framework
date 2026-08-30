/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/bash.c
 *
 * PURPOSE:
 *   Define Framework-owned Bash Language Server executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/bash.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_bash(void){static const UmiLanguageServerProfile p={.id="language.server.bash",.display_name="Bash Language Server",.executable="bash-language-server",.arguments="start",.language_ids="bash,shellscript",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
