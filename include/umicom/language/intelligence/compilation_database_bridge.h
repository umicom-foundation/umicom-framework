/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/intelligence/compilation_database_bridge.h
 *
 * PURPOSE:
 *   Associate projects with compile_commands-style compilation databases.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable language-intelligence capability. Studio,
 *   Desk and every product remain thin consumers of Framework state/contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_COMPILATION_DATABASE_BRIDGE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_COMPILATION_DATABASE_BRIDGE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_COMPILATION_DATABASE_BRIDGE_API_VERSION 1U

typedef struct UmiLanguageIntelligenceCompilationDatabaseBridge {
    uint32_t struct_size;
    uint32_t api_version;
    char source_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char target_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    char scope_id[UMI_LANGUAGE_INTELLIGENCE_ID_CAPACITY];
    uint32_t priority;
    int enabled;
    uint64_t revision;
} UmiLanguageIntelligenceCompilationDatabaseBridge;

void umi_language_intelligence_compilation_database_bridge_init(UmiLanguageIntelligenceCompilationDatabaseBridge *mapping);
UmiStatus umi_language_intelligence_compilation_database_bridge_set(
    UmiLanguageIntelligenceCompilationDatabaseBridge *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
UmiStatus umi_language_intelligence_compilation_database_bridge_validate(
    const UmiLanguageIntelligenceCompilationDatabaseBridge *mapping);
int umi_language_intelligence_compilation_database_bridge_matches(
    const UmiLanguageIntelligenceCompilationDatabaseBridge *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
