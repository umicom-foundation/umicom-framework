/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/profiles/cmake.c
 *
 * PURPOSE:
 *   Define Framework-owned CMake Language Server executable and capability metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/profiles/cmake.h"
const UmiLanguageServerProfile *umi_language_runtime_profile_cmake(void){static const UmiLanguageServerProfile p={.id="language.server.cmake",.display_name="CMake Language Server",.executable="cmake-language-server",.arguments="",.language_ids="cmake",.capabilities=UMI_LANGUAGE_CAP_COMPLETION | UMI_LANGUAGE_CAP_HOVER | UMI_LANGUAGE_CAP_DEFINITION | UMI_LANGUAGE_CAP_SYMBOLS,.enabled=1,.autostart=1,.revision=1U};return &p;}
