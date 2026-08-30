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
typedef struct UmiPolyglotCompilationEntry { char unit_id[UMI_COMPILER_ID_CAPACITY]; char directory[UMI_COMPILER_PATH_CAPACITY]; char source[UMI_COMPILER_PATH_CAPACITY]; char output[UMI_COMPILER_PATH_CAPACITY]; UmiCompilerLanguage language; UmiCompilerPlan plan; } UmiPolyglotCompilationEntry;
typedef struct UmiPolyglotCompilationDatabase { UmiPolyglotCompilationEntry items[UMI_POLYGLOT_DATABASE_MAX]; size_t count; uint64_t revision; } UmiPolyglotCompilationDatabase;
UmiStatus umi_polyglot_compilation_database_add(UmiPolyglotCompilationDatabase *database,const UmiPolyglotCompilationEntry *entry);
const UmiPolyglotCompilationEntry *umi_polyglot_compilation_database_find(const UmiPolyglotCompilationDatabase *database,const char *source);
UmiStatus umi_polyglot_compilation_database_export_json(const UmiPolyglotCompilationDatabase *database,char *out_json,size_t capacity);
#endif
