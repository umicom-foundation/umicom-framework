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
#endif
