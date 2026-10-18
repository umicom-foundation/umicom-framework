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
#include "umicom/base/value_archive.h"
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_language_intelligence_compilation_database_bridge_archive_encode(const UmiLanguageIntelligenceCompilationDatabaseBridge *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_language_intelligence_compilation_database_bridge_archive_decode(const void *bytes, size_t byte_count,
    UmiLanguageIntelligenceCompilationDatabaseBridge *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiLanguageIntelligenceCompilationDatabaseBridge object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_language_intelligence_compilation_database_bridge_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiLanguageIntelligenceCompilationDatabaseBridge *workspace, UmiLanguageIntelligenceCompilationDatabaseBridge *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_language_intelligence_compilation_database_bridge_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiLanguageIntelligenceCompilationDatabaseBridge *expected, UmiLanguageIntelligenceCompilationDatabaseBridge *value);


#ifdef __cplusplus
}
#endif
#endif
