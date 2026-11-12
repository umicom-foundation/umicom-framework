/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/compilation_database.h
 *
 * PURPOSE:
 *   Publish the public compilation database contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_COMPILATION_DATABASE_H
#define UMICOM_COMPILER_COMPILATION_DATABASE_H
#include "umicom/compiler/plan.h"

/* Keep the public C symbol names when this header is included directly by a
 * C++ client. Includes remain outside the linkage block; layouts are unchanged. */
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_POLYGLOT_DATABASE_MAX 512U
/**
 * Represent the polyglot compilation entry data shared with callers of this public
 * contract.
 */
typedef struct UmiPolyglotCompilationEntry { char unit_id[UMI_COMPILER_ID_CAPACITY]; char directory[UMI_COMPILER_PATH_CAPACITY]; char source[UMI_COMPILER_PATH_CAPACITY]; char output[UMI_COMPILER_PATH_CAPACITY]; UmiCompilerLanguage language; UmiCompilerPlan plan; } UmiPolyglotCompilationEntry;
/**
 * Represent the polyglot compilation database data shared with callers of this public
 * contract.
 */
typedef struct UmiPolyglotCompilationDatabase { UmiPolyglotCompilationEntry items[UMI_POLYGLOT_DATABASE_MAX]; size_t count; uint64_t revision; } UmiPolyglotCompilationDatabase;
/**
 * Add polyglot compilation database only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_polyglot_compilation_database_add(UmiPolyglotCompilationDatabase *database,const UmiPolyglotCompilationEntry *entry);
/**
 * Find polyglot compilation database while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiPolyglotCompilationEntry *umi_polyglot_compilation_database_find(const UmiPolyglotCompilationDatabase *database,const char *source);
/**
 * Provide the polyglot compilation database export json operation used by this module and
 * its client applications.
 */
UmiStatus umi_polyglot_compilation_database_export_json(const UmiPolyglotCompilationDatabase *database,char *out_json,size_t capacity);
/** Check required unit/source identity, bounded optional directory/output,
 * a known entry language and all nested inline plan storage. Draft plan metadata
 * is permitted for composition. Cached argument pointers are not dereferenced. */
bool umi_polyglot_compilation_entry_valid(const UmiPolyglotCompilationEntry *entry);
/** Admit the count and all active entries before lookup, insertion or export.
 * Insertion copies the value and rebases stored plan views. Refusal preserves
 * every byte; existing source duplicates precede slot/revision exhaustion.
 * Lookup returns borrowed storage. If a whole database is copied by value,
 * rebase each copied plan before using its argument_view. Callers serialize
 * access and keep owners alive. Inactive slots and filesystem state are ignored.
 *
 * Export additionally requires a nonempty directory, executable and at least
 * one driver argument in every entry. It prepends the executable to the ordered
 * arguments array, validates emitted UTF-8 and preserves the complete output
 * buffer on every refusal. Output must not overlap the database. This describes
 * commands; it does not run them or qualify the toolchain. The existing
 * one-entry-per-source duplicate policy remains; multiple configurations for a
 * source require a separate database. Optional output and language fields remain.
 */
bool umi_polyglot_compilation_database_valid(const UmiPolyglotCompilationDatabase *database);

/** Insert or replace the row with the same literal source string. Unlike add,
 * this deliberately refreshes an existing command, even when all slots are used.
 * Every successful call increments revision once and rebases stored plan views.
 * Exhausted revision or other refusal preserves the complete database.
 * No path normalization, multi-configuration merge or concurrency is provided. */
UmiStatus umi_polyglot_compilation_database_upsert(UmiPolyglotCompilationDatabase *database,const UmiPolyglotCompilationEntry *entry);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif
