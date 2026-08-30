/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/jdtls.c
 *
 * PURPOSE:
 *   Define Framework-owned Eclipse JDT Language Server executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/jdtls.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_jdtls(void){static const UmiLanguageServerProfile p={.id="language.server.jdtls",.display_name="Eclipse JDT Language Server",.executable="jdtls",.arguments="",.language_ids="java",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SIGNATURE | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_RENAME | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_CODE_ACTION | UMI_LANGUAGE_CAP_SEMANTIC_TOKENS | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
