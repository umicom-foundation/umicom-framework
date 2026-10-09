/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/compilation_database.h
 * PURPOSE: Inspect compiler commands as owned metadata without executing the commands.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DEVELOPER_PROJECT_COMPILATION_DATABASE_H
#define UMICOM_DEVELOPER_PROJECT_COMPILATION_DATABASE_H
#include "umicom/build/profile.h"
#include "umicom/platform/cancellation.h"
#include <stdbool.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_COMPILATION_DATABASE_BYTE_LIMIT (64U * 1024U * 1024U)
#define UMI_COMPILATION_DATABASE_ENTRY_LIMIT 65536U
#define UMI_COMPILATION_DATABASE_ARGUMENT_LIMIT 4096U
#define UMI_COMPILATION_DATABASE_TEXT_LIMIT 65535U
    typedef struct UmiCompilationDatabase UmiCompilationDatabase;
    typedef struct UmiCompilationCommand
    {
        char directory[UMI_BUILD_PATH_CAPACITY];
        char source_file[UMI_BUILD_PATH_CAPACITY];
        char output_file[UMI_BUILD_PATH_CAPACITY];
        size_t argument_count;
        bool has_arguments;
        bool has_command;
    } UmiCompilationCommand;
    /** Copy and validate a complete JSON compilation database. The root must be an
 * array of command objects. Directory is absolute; file and output paths are
 * resolved against that directory, never the host's working directory.
 * Both command and arguments may be present. Commands are opaque text, never
 * parsed by a shell or executed. Each argument/command is at most TEXT_LIMIT
 * UTF-8 bytes. Empty arguments are valid, but argv[0] and command cannot be empty.
 * Repeated source files are retained as distinct build variants.
 * Limits bound bytes, entries, arguments and the shared JSON tree's node/depth
 * budget. Cancellation is cooperative between records and arguments. Failure
 * clears out; no partial database is returned. This does not inspect source,
 * compiler or output files, infer freshness, or grant trust. */
    UmiStatus UmiCompilationDatabaseCreate(const void *bytes, size_t length,
                                           const UmiCancellationToken *cancel,
                                           UmiCompilationDatabase **out);
    /** Read compile_commands.json from an explicitly selected absolute directory.
 * Uses the bounded regular-file reader; final symlink/device leaves are refused.
 * Parent aliases may be followed. This can block; UI hosts must use a worker.
 * No configure, process, directory creation or source modification occurs. */
    UmiStatus UmiCompilationDatabaseRead(const char *directory, const UmiCancellationToken *cancel,
                                         UmiCompilationDatabase **out);
    /** Release the snapshot and its owned JSON bytes. NULL is accepted. */
    void UmiCompilationDatabaseDestroy(UmiCompilationDatabase *database);
    /** Return command rows, including multiple variants for the same source file. */
    size_t UmiCompilationDatabaseCount(const UmiCompilationDatabase *database);
    /** Copy one normalised record. Out remains unchanged on failure. */
    UmiStatus UmiCompilationDatabaseAt(const UmiCompilationDatabase *database, size_t index,
                                       UmiCompilationCommand *out);
    /** Copy one decoded argument. NOT_FOUND means the row has no arguments or this
 * argument is absent. Output remains unchanged on failure. */
    UmiStatus UmiCompilationDatabaseArgument(const UmiCompilationDatabase *database, size_t index,
                                             size_t argument, char *out, size_t capacity);
    /** Copy the original decoded command string, without interpreting quoting.
 * NOT_FOUND means arguments were supplied without command. Output is unchanged
 * on failure. Display this text for review; do not turn it into a shell action. */
    UmiStatus UmiCompilationDatabaseCommand(const UmiCompilationDatabase *database, size_t index,
                                            char *out, size_t capacity);
    /** Find the next matching source starting at start_index. Pass returned index+1
 * to inspect another configuration. Path comparison follows host semantics and
 * lexical normalisation, without resolving symlinks. Output changes on success
 * only; a relative source path is invalid. */
    UmiStatus UmiCompilationDatabaseFind(const UmiCompilationDatabase *database,
                                         const char *absolute_source, size_t start_index,
                                         size_t *out_index);
    /** Prepare a profile with -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON, replacing an
 * existing explicit definition of that name and preserving all other settings.
 * This is an explicit draft edit, not configure or persistence. CMake supports
 * export with Makefile/Ninja generators; presets still choose their generator.
 * Failure leaves out unchanged; input and output may be the same profile. */
    UmiStatus UmiCompilationDatabaseEnableExport(const UmiBuildProfile *profile,
                                                 UmiBuildProfile *out);
    /** Prepare clangd argument text with one --compile-commands-dir= value.
 * This helper is explicitly for clangd; it does not identify an executable.
 * Preserve other parsed arguments. Replace an existing single- or double-dash
 * spelling, including a separate value. Refuse duplicates, response files and
 * an end-of-options marker because their effective options cannot be inspected.
 * No file read or server start occurs; a caller must review the database first.
 * Output remains unchanged on failure; input/output may alias. */
    UmiStatus UmiCompilationDatabaseClangdArguments(const char *arguments,
                                                    const char *absolute_directory, char *out,
                                                    size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
