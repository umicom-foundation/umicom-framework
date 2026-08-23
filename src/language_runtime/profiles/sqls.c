/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/sqls.c
 *
 * PURPOSE:
 *   Define Framework-owned SQL Language Server executable and capability metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/sqls.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_sqls(void){static const UmiLanguageServerProfile p={.id="language.server.sqls",.display_name="SQL Language Server",.executable="sqls",.arguments="",.language_ids="sql",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_REFERENCES | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_FORMATTING,.enabled=1,.autostart=1,.revision=1U};return &p;}
