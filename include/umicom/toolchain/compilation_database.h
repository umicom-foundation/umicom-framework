/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/compilation_database.h
 *
 * PURPOSE:
 *   Import bounded compile_commands.json metadata for project-aware compiler,
 *   source-file and build-directory selection without running shell commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_COMPILATION_DATABASE_H
#define UMICOM_TOOLCHAIN_COMPILATION_DATABASE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/capability.h"
#include "umicom/toolchain/tool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_COMPILATION_DATABASE_API_VERSION 1U
#define UMI_COMPILATION_DATABASE_CAPACITY 512U

/**
 * Represent the compilation command snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiCompilationCommandSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char directory[UMI_TOOL_PATH_CAPACITY];
    char file[UMI_TOOL_PATH_CAPACITY];
    char output[UMI_TOOL_PATH_CAPACITY];
    char compiler[UMI_TOOL_PATH_CAPACITY];
    UmiCompilerVendor compiler_vendor;
    uint64_t revision;
} UmiCompilationCommandSnapshot;

/**
 * Represent the compilation database snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiCompilationDatabaseSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char path[UMI_TOOL_PATH_CAPACITY];
    uint64_t revision;
    size_t command_count;
    size_t clang_command_count;
    size_t gcc_command_count;
    size_t msvc_command_count;
    size_t unknown_command_count;
    size_t truncated_command_count;
} UmiCompilationDatabaseSnapshot;

/**
 * Represent the compilation database data shared with callers of this public contract.
 */
typedef struct UmiCompilationDatabase UmiCompilationDatabase;

/**
 * Initialise compilation database from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_compilation_database_create(
    UmiCompilationDatabase **out_database);
/**
 * Release or reset state held by compilation database so the same storage can be reused
 * safely.
 */
void umi_compilation_database_destroy(UmiCompilationDatabase *database);
/**
 * Read compilation database into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_compilation_database_load(
    UmiCompilationDatabase *database,
    const char *path);
/**
 * Provide the compilation database import json operation used by this module and its
 * client applications.
 */
UmiStatus umi_compilation_database_import_json(
    UmiCompilationDatabase *database,
    const char *json,
    const char *origin,
    size_t *out_imported);
/**
 * Provide the compilation database discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_compilation_database_discover(
    const char *project_root,
    const char *const *candidate_build_directories,
    size_t candidate_build_directory_count,
    char *out_path,
    size_t capacity);
/**
 * Provide the compilation database snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_compilation_database_snapshot(
    const UmiCompilationDatabase *database,
    UmiCompilationDatabaseSnapshot *out_snapshot);
/**
 * Find compilation database command while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_compilation_database_command_at(
    const UmiCompilationDatabase *database,
    size_t index,
    UmiCompilationCommandSnapshot *out_command);
/**
 * Provide the compilation database find file operation used by this module and its client
 * applications.
 */
UmiStatus umi_compilation_database_find_file(
    const UmiCompilationDatabase *database,
    const char *file,
    UmiCompilationCommandSnapshot *out_command);

#ifdef __cplusplus
}
#endif
#endif
