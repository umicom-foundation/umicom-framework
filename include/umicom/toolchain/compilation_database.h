/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/compilation_database.h
 *
 * PURPOSE:
 *   Import bounded compile_commands.json metadata for project-aware compiler,
 *   source-file and build-directory selection without running shell commands.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiCompilationDatabase UmiCompilationDatabase;

UmiStatus umi_compilation_database_create(
    UmiCompilationDatabase **out_database);
void umi_compilation_database_destroy(UmiCompilationDatabase *database);
UmiStatus umi_compilation_database_load(
    UmiCompilationDatabase *database,
    const char *path);
UmiStatus umi_compilation_database_discover(
    const char *project_root,
    const char *const *candidate_build_directories,
    size_t candidate_build_directory_count,
    char *out_path,
    size_t capacity);
UmiStatus umi_compilation_database_snapshot(
    const UmiCompilationDatabase *database,
    UmiCompilationDatabaseSnapshot *out_snapshot);
UmiStatus umi_compilation_database_command_at(
    const UmiCompilationDatabase *database,
    size_t index,
    UmiCompilationCommandSnapshot *out_command);
UmiStatus umi_compilation_database_find_file(
    const UmiCompilationDatabase *database,
    const char *file,
    UmiCompilationCommandSnapshot *out_command);

#ifdef __cplusplus
}
#endif
#endif
