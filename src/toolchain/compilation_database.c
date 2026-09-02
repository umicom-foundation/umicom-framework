/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/compilation_database.c
 *
 * PURPOSE:
 *   Import compile_commands.json using a bounded dependency-free JSON scanner.
 *   Only metadata needed by the toolchain centre is retained; command strings
 *   are never executed and shell concatenation is deliberately avoided.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/compilation_database.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

struct UmiCompilationDatabase {
    UmiCompilationCommandSnapshot items[UMI_COMPILATION_DATABASE_CAPACITY];
    size_t count;
    size_t truncated_count;
    char path[UMI_TOOL_PATH_CAPACITY];
    uint64_t revision;
};

/* Provide the skip space operation used by this module and its client applications. */
static const char *skip_space(const char *cursor)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && *cursor != '\0' &&
           isspace((unsigned char)*cursor)) cursor += 1;
    return cursor;
}

/* Provide the scan json string operation used by this module and its client applications. */
static const char *scan_json_string(const char *cursor,
                                    char *out_text, size_t capacity)
{
    size_t used = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || *cursor != '"' || out_text == NULL || capacity == 0U)
        return NULL;
    cursor += 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' && *cursor != '"') {
        char value = *cursor++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (value == '\\') {
            value = *cursor++;
            /* Select the behaviour associated with the requested command or state value. */
            switch (value) {
                case '"': value = '"'; break;
                case '\\': value = '\\'; break;
                case '/': value = '/'; break;
                case 'b': value = '\b'; break;
                case 'f': value = '\f'; break;
                case 'n': value = '\n'; break;
                case 'r': value = '\r'; break;
                case 't': value = '\t'; break;
                default: return NULL;
            }
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (used + 1U >= capacity) return NULL;
        out_text[used++] = value;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (*cursor != '"') return NULL;
    out_text[used] = '\0';
    return cursor + 1;
}

/* Provide the skip json string operation used by this module and its client applications. */
static const char *skip_json_string(const char *cursor)
{
    int escaped = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (cursor == NULL || *cursor != '"') return NULL;
    cursor += 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (escaped) {
            escaped = 0;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '\\') {
            escaped = 1;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '"') {
            return cursor + 1;
        }
        cursor += 1;
    }
    return NULL;
}

/* Provide the find field operation used by this module and its client applications. */
static const char *find_field(const char *object, const char *object_end,
                              const char *field)
{
    char key[256];
    const char *cursor = object;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (cursor != NULL && cursor < object_end && *cursor != '\0') {
        cursor = skip_space(cursor);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (cursor >= object_end) break;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '"') {
            const char *after = skip_json_string(cursor);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (after == NULL || after > object_end) return NULL;
            after = skip_space(after);
            /* Apply this branch only when its contract condition is satisfied. */
            if (after < object_end && *after == ':') {
                /*
                 * Protect caller-owned memory by checking that required state is available before it is
                 * used.
                 */
                if (scan_json_string(cursor, key, sizeof(key)) == NULL)
                    return NULL;
                /* Use the stable identifier comparison to choose the matching record or policy. */
                if (strcmp(key, field) == 0) return skip_space(after + 1);
            }
            cursor = after;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            cursor += 1;
        }
    }
    return NULL;
}

/* Provide the find object end operation used by this module and its client applications. */
static const char *find_object_end(const char *object)
{
    const char *cursor;
    size_t depth = 0U;
    int in_string = 0;
    int escaped = 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = object; cursor != NULL && *cursor != '\0'; ++cursor) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (in_string) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (escaped) escaped = 0;
            else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '\\') escaped = 1;
            else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '"') in_string = 0;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '"') in_string = 1;
        else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '{') depth += 1U;
        else /* Apply this branch only when its contract condition is satisfied. */ if (*cursor == '}') {
            /* Apply this branch only when its contract condition is satisfied. */
            if (depth == 0U) return NULL;
            depth -= 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (depth == 0U) return cursor + 1;
        }
    }
    return NULL;
}

/* Provide the read string field operation used by this module and its client applications. */
static int read_string_field(const char *object, const char *object_end,
                             const char *field, char *out_text, size_t capacity)
{
    const char *value = find_field(object, object_end, field);
    return value != NULL && value < object_end && *value == '"' &&
           scan_json_string(value, out_text, capacity) != NULL;
}

/*
 * Provide the first program token operation used by this module and its client
 * applications.
 */
static void first_program_token(const char *command,
                                char *out_program, size_t capacity)
{
    const char *cursor = skip_space(command);
    size_t used = 0U;
    char quote = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_program == NULL || capacity == 0U) return;
    out_program[0] = '\0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (*cursor == '"' || *cursor == '\'') quote = *cursor++;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' &&
           ((quote != '\0' && *cursor != quote) ||
            (quote == '\0' && !isspace((unsigned char)*cursor)))) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (used + 1U >= capacity) return;
        out_program[used++] = *cursor++;
    }
    out_program[used] = '\0';
}

/* Provide the compiler vendor operation used by this module and its client applications. */
static UmiCompilerVendor compiler_vendor(const char *compiler)
{
    char name[UMI_PATH_CAPACITY];
    size_t index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_path_basename(compiler, name, sizeof(name)) != UMI_STATUS_OK)
        (void)snprintf(name, sizeof(name), "%s", compiler != NULL ? compiler : "");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; name[index] != '\0'; ++index)
        name[index] = (char)tolower((unsigned char)name[index]);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(name, "clang") != NULL) return UMI_COMPILER_VENDOR_CLANG;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(name, "gcc") != NULL || strstr(name, "g++") != NULL)
        return UMI_COMPILER_VENDOR_GCC;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(name, "cl") == 0 || strcmp(name, "cl.exe") == 0)
        return UMI_COMPILER_VENDOR_MSVC;
    return UMI_COMPILER_VENDOR_UNKNOWN;
}

/*
 * Provide the compiler from arguments operation used by this module and its client
 * applications.
 */
static int compiler_from_arguments(const char *object, const char *object_end,
                                   char *out_compiler, size_t capacity)
{
    const char *value = find_field(object, object_end, "arguments");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value >= object_end || *value != '[') return 0;
    value = skip_space(value + 1);
    return value < object_end && *value == '"' &&
           scan_json_string(value, out_compiler, capacity) != NULL;
}

/* Provide the add object operation used by this module and its client applications. */
static UmiStatus add_object(UmiCompilationDatabase *database,
                            const char *object, const char *object_end)
{
    UmiCompilationCommandSnapshot *item;
    char command[UMI_TOOL_PATH_CAPACITY];
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (database->count >= UMI_COMPILATION_DATABASE_CAPACITY) {
        database->truncated_count += 1U;
        return UMI_STATUS_OK;
    }
    item = &database->items[database->count];
    (void)memset(item, 0, sizeof(*item));
    item->struct_size = (uint32_t)sizeof(*item);
    item->api_version = UMI_COMPILATION_DATABASE_API_VERSION;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!read_string_field(object, object_end, "directory",
                           item->directory, sizeof(item->directory)) ||
        !read_string_field(object, object_end, "file",
                           item->file, sizeof(item->file)))
        return UMI_STATUS_PARSE_ERROR;
    (void)read_string_field(object, object_end, "output",
                            item->output, sizeof(item->output));
    /* Apply this branch only when its contract condition is satisfied. */
    if (read_string_field(object, object_end, "command",
                          command, sizeof(command)))
        first_program_token(command, item->compiler, sizeof(item->compiler));
    else /* Apply this branch only when its contract condition is satisfied. */ if (!compiler_from_arguments(object, object_end,
                                      item->compiler, sizeof(item->compiler)))
        return UMI_STATUS_PARSE_ERROR;
    item->compiler_vendor = compiler_vendor(item->compiler);
    item->revision = database->revision + 1U;
    database->count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Initialise compilation database from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_compilation_database_create(
    UmiCompilationDatabase **out_database)
{
    UmiCompilationDatabase *database;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_database == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_database = NULL;
    database = (UmiCompilationDatabase *)calloc(1U, sizeof(*database));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    database->revision = 1U;
    *out_database = database;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by compilation database so the same storage can be reused
 * safely.
 */
void umi_compilation_database_destroy(UmiCompilationDatabase *database)
{
    free(database);
}

/*
 * Read compilation database into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_compilation_database_load(
    UmiCompilationDatabase *database, const char *path)
{
    char *text = NULL;
    size_t size = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database == NULL || path == NULL || path[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_fs_read_text(path, &text, &size);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK && size == 0U) status = UMI_STATUS_PARSE_ERROR;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_compilation_database_import_json(database, text, path,
                                                      NULL);
    }
    umi_fs_free_text(text);
    return status;
}

/*
 * Provide the compilation database import json operation used by this module and its
 * client applications.
 */
UmiStatus umi_compilation_database_import_json(
    UmiCompilationDatabase *database,
    const char *json,
    const char *origin,
    size_t *out_imported)
{
    UmiCompilationDatabase *pending;
    const char *cursor;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database == NULL || json == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    pending = (UmiCompilationDatabase *)calloc(1U, sizeof(*pending));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pending == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    pending->revision = database->revision;
    cursor = skip_space(json);
    /* Apply this branch only when its contract condition is satisfied. */
    if (*cursor != '[') {
        free(pending);
        return UMI_STATUS_PARSE_ERROR;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (origin != NULL && origin[0] != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (strlen(origin) + 1U > sizeof(pending->path)) {
            free(pending);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        (void)snprintf(pending->path, sizeof(pending->path), "%s", origin);
    }
    cursor += 1;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*(cursor = skip_space(cursor)) != '\0' && *cursor != ']') {
        const char *object_end;
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == ',') {
            cursor += 1;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor != '{') {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        object_end = find_object_end(cursor);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (object_end == NULL) {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        status = add_object(pending, cursor, object_end);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        cursor = object_end;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        pending->revision += 1U;
        *database = *pending;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_imported != NULL) *out_imported = database->count;
    } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (out_imported != NULL) {
        *out_imported = 0U;
    }
    free(pending);
    return status;
}

/*
 * Provide the find compilation database operation used by this module and its client
 * applications.
 */
static UmiStatus find_compilation_database(const char *directory,
                                           char *out_path,
                                           size_t capacity)
{
    char candidate[UMI_TOOL_PATH_CAPACITY];
    UmiStatus status;
    status = umi_fs_join(candidate, sizeof(candidate), directory,
                         "compile_commands.json");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_fs_is_file(candidate)) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(candidate) + 1U > capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(out_path, capacity, "%s", candidate);
    return UMI_STATUS_OK;
}

/*
 * Provide the compilation database discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_compilation_database_discover(
    const char *project_root,
    const char *const *candidate_build_directories,
    size_t candidate_build_directory_count,
    char *out_path,
    size_t capacity)
{
    static const char *const conventional_directories[] = {
        "build", "build/debug", "build/headless-debug",
        "build/windows-ucrt64-headless-debug",
        "build/windows-ucrt64-debug", "out/build"
    };
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_root == NULL || project_root[0] == '\0' || out_path == NULL ||
        capacity == 0U ||
        (candidate_build_directory_count != 0U &&
         candidate_build_directories == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    out_path[0] = '\0';
    status = find_compilation_database(project_root, out_path, capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK || status == UMI_STATUS_CAPACITY_EXCEEDED)
        return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < candidate_build_directory_count; ++index) {
        char directory[UMI_TOOL_PATH_CAPACITY];
        const char *item = candidate_build_directories[index];
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (item == NULL || item[0] == '\0') continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_path_is_absolute(item)) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (strlen(item) + 1U > sizeof(directory))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            (void)snprintf(directory, sizeof(directory), "%s", item);
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            status = umi_fs_join(directory, sizeof(directory), project_root,
                                 item);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
        status = find_compilation_database(directory, out_path, capacity);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK || status == UMI_STATUS_CAPACITY_EXCEEDED)
            return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < sizeof(conventional_directories) /
                     sizeof(conventional_directories[0]);
         ++index) {
        char directory[UMI_TOOL_PATH_CAPACITY];
        status = umi_fs_join(directory, sizeof(directory), project_root,
                             conventional_directories[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = find_compilation_database(directory, out_path, capacity);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK || status == UMI_STATUS_CAPACITY_EXCEEDED)
            return status;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the compilation database snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_compilation_database_snapshot(
    const UmiCompilationDatabase *database,
    UmiCompilationDatabaseSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_COMPILATION_DATABASE_API_VERSION;
    (void)snprintf(out_snapshot->path, sizeof(out_snapshot->path), "%s",
                   database->path);
    out_snapshot->revision = database->revision;
    out_snapshot->command_count = database->count;
    out_snapshot->truncated_command_count = database->truncated_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < database->count; ++index) {
        /* Select the behaviour associated with the requested command or state value. */
        switch (database->items[index].compiler_vendor) {
            case UMI_COMPILER_VENDOR_CLANG:
                out_snapshot->clang_command_count += 1U; break;
            case UMI_COMPILER_VENDOR_GCC:
                out_snapshot->gcc_command_count += 1U; break;
            case UMI_COMPILER_VENDOR_MSVC:
                out_snapshot->msvc_command_count += 1U; break;
            default:
                out_snapshot->unknown_command_count += 1U; break;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Find compilation database command while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_compilation_database_command_at(
    const UmiCompilationDatabase *database, size_t index,
    UmiCompilationCommandSnapshot *out_command)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database == NULL || out_command == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= database->count) return UMI_STATUS_NOT_FOUND;
    *out_command = database->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the compilation database find file operation used by this module and its client
 * applications.
 */
UmiStatus umi_compilation_database_find_file(
    const UmiCompilationDatabase *database, const char *file,
    UmiCompilationCommandSnapshot *out_command)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database == NULL || file == NULL || out_command == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < database->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_path_equal(database->items[index].file, file)) {
            *out_command = database->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
