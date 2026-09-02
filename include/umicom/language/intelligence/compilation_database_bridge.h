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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_INTELLIGENCE_COMPILATION_DATABASE_BRIDGE_H
#define UMICOM_LANGUAGE_INTELLIGENCE_COMPILATION_DATABASE_BRIDGE_H
#include "umicom/language/intelligence/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_INTELLIGENCE_COMPILATION_DATABASE_BRIDGE_API_VERSION 1U

/**
 * Represent the language intelligence compilation database bridge data shared with callers
 * of this public contract.
 */
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

/**
 * Initialise language intelligence compilation database bridge from caller-provided values
 * so later operations receive a known state.
 */
void umi_language_intelligence_compilation_database_bridge_init(UmiLanguageIntelligenceCompilationDatabaseBridge *mapping);
/**
 * Copy language intelligence compilation database bridge into module-owned storage so
 * callers keep ownership of their input values.
 */
UmiStatus umi_language_intelligence_compilation_database_bridge_set(
    UmiLanguageIntelligenceCompilationDatabaseBridge *mapping,
    const char *source_id,
    const char *target_id,
    const char *scope_id);
/**
 * Check that language intelligence compilation database bridge satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_language_intelligence_compilation_database_bridge_validate(
    const UmiLanguageIntelligenceCompilationDatabaseBridge *mapping);
/**
 * Provide the language intelligence compilation database bridge matches operation used by
 * this module and its client applications.
 */
int umi_language_intelligence_compilation_database_bridge_matches(
    const UmiLanguageIntelligenceCompilationDatabaseBridge *mapping,
    const char *source_id,
    const char *scope_id);
#ifdef __cplusplus
}
#endif
#endif
