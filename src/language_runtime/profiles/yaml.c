/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/yaml.c
 *
 * PURPOSE:
 *   Define Framework-owned YAML Language Server executable and capability metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/yaml.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_yaml(void){static const UmiLanguageServerProfile p={.id="language.server.yaml",.display_name="YAML Language Server",.executable="yaml-language-server",.arguments="--stdio",.language_ids="yaml",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_SYMBOLS | UMI_LANGUAGE_CAP_FORMATTING | UMI_LANGUAGE_CAP_CODE_ACTION | UMI_LANGUAGE_CAP_FOLDING,.enabled=1,.autostart=1,.revision=1U};return &p;}
