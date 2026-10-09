/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/compilation_database.c
 * PURPOSE: Validate every compiler-command row before publishing an immutable snapshot.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "compilation_database_internal.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/path.h"
#include <stdlib.h>
#include <string.h>

/* Resolve fields through decoded keys, so an escaped duplicate spelling cannot
 * override a reviewed path. Unknown fields remain validated JSON metadata. */
static UmiStatus CompilationField(const UmiJsonTree *tree, int row, const char *name, bool required,
                                  int *out)
{
    UmiStatus status = UmiJsonTreeMember(tree, row, name, out);
    if (status == UMI_STATUS_NOT_FOUND)
    {
        *out = -1;
        return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    }
    return status == UMI_STATUS_ALREADY_EXISTS ? UMI_STATUS_PARSE_ERROR : status;
}
static UmiStatus CompilationPath(const UmiJsonTree *tree, int row, const char *name,
                                 const char *base, bool required, char *out)
{
    int node = -1;
    UmiStatus status = CompilationField(tree, row, name, required, &node);
    char text[UMI_BUILD_PATH_CAPACITY];
    if (status != UMI_STATUS_OK || node == -1)
        return status;
    status = UmiJsonTreeText(tree, node, text, sizeof text);
    if (status != UMI_STATUS_OK)
        return status;
    if (text[0] == '\0' || (base == NULL && !umi_path_is_absolute(text)))
        return UMI_STATUS_PARSE_ERROR;
    status = base == NULL ? umi_path_normalise(text, out, UMI_BUILD_PATH_CAPACITY)
                          : umi_path_absolute(text, base, out, UMI_BUILD_PATH_CAPACITY);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(out);
    return status;
}
UmiStatus UmiCompilationDatabaseRecord(const UmiCompilationDatabase *database, int row,
                                       UmiCompilationCommand *out)
{
    if (UmiJsonTreeKind(database->tree, row) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    UmiCompilationCommand record = {0};
    UmiStatus status =
        CompilationPath(database->tree, row, "directory", NULL, true, record.directory);
    if (status == UMI_STATUS_OK)
        status = CompilationPath(database->tree, row, "file", record.directory, true,
                                 record.source_file);
    if (status == UMI_STATUS_OK)
        status = CompilationPath(database->tree, row, "output", record.directory, false,
                                 record.output_file);
    int arguments = -1, command = -1;
    if (status == UMI_STATUS_OK)
        status = CompilationField(database->tree, row, "arguments", false, &arguments);
    if (status == UMI_STATUS_OK)
        status = CompilationField(database->tree, row, "command", false, &command);
    if (status != UMI_STATUS_OK)
        return status;
    if (arguments == -1 && command == -1)
        return UMI_STATUS_PARSE_ERROR;
    if (arguments != -1)
    {
        if (UmiJsonTreeKind(database->tree, arguments) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        record.argument_count = UmiJsonTreeCount(database->tree, arguments);
        if (record.argument_count == 0U)
            return UMI_STATUS_PARSE_ERROR;
        if (record.argument_count > UMI_COMPILATION_DATABASE_ARGUMENT_LIMIT)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        record.has_arguments = true;
    }
    if (command != -1)
    {
        if (UmiJsonTreeKind(database->tree, command) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
            return UMI_STATUS_PARSE_ERROR;
        record.has_command = true;
    }
    *out = record;
    return UMI_STATUS_OK;
}
/* The largest command cell is shared for validation rather than placed on the
 * stack per row. Keeping row handles avoids an array of large path records. */
static UmiStatus CompilationValidateTexts(const UmiCompilationDatabase *database, int row,
                                          char *scratch, const UmiCancellationToken *cancel)
{
    int arguments = -1, command = -1;
    UmiStatus status = CompilationField(database->tree, row, "arguments", false, &arguments);
    if (status == UMI_STATUS_OK)
        status = CompilationField(database->tree, row, "command", false, &command);
    if (status == UMI_STATUS_OK && command != -1)
    {
        status = UmiJsonTreeText(database->tree, command, scratch,
                                 UMI_COMPILATION_DATABASE_TEXT_LIMIT + 1U);
        if (status == UMI_STATUS_OK && scratch[0] == '\0')
            status = UMI_STATUS_PARSE_ERROR;
    }
    bool first = true;
    for (int node = UmiJsonTreeFirst(database->tree, arguments);
         status == UMI_STATUS_OK && node != -1; node = UmiJsonTreeNext(database->tree, node))
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        status = UmiJsonTreeText(database->tree, node, scratch,
                                 UMI_COMPILATION_DATABASE_TEXT_LIMIT + 1U);
        if (status == UMI_STATUS_OK && first && scratch[0] == '\0')
            status = UMI_STATUS_PARSE_ERROR;
        first = false;
    }
    return status;
}
UmiStatus UmiCompilationDatabaseCreate(const void *bytes, size_t length,
                                       const UmiCancellationToken *cancel,
                                       UmiCompilationDatabase **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCompilationDatabase *database = calloc(1U, sizeof *database);
    if (database == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonTreeLimits limits = {UMI_COMPILATION_DATABASE_BYTE_LIMIT, 2097152U, 128U};
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, cancel, &database->tree);
    if (status == UMI_STATUS_OK &&
        UmiJsonTreeKind(database->tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
    {
        database->count = UmiJsonTreeCount(database->tree, 0);
        if (database->count > UMI_COMPILATION_DATABASE_ENTRY_LIMIT)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char *scratch = NULL;
    if (status == UMI_STATUS_OK && database->count != 0U)
    {
        database->rows = calloc(database->count, sizeof *database->rows);
        scratch = malloc(UMI_COMPILATION_DATABASE_TEXT_LIMIT + 1U);
        if (database->rows == NULL || scratch == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    size_t index = 0U;
    for (int row = UmiJsonTreeFirst(database->tree, 0); status == UMI_STATUS_OK && row != -1;
         row = UmiJsonTreeNext(database->tree, row))
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        UmiCompilationCommand record;
        status = UmiCompilationDatabaseRecord(database, row, &record);
        if (status == UMI_STATUS_OK)
            status = CompilationValidateTexts(database, row, scratch, cancel);
        if (status == UMI_STATUS_OK)
            database->rows[index++] = row;
    }
    free(scratch);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = database;
    else
        UmiCompilationDatabaseDestroy(database);
    return status;
}
UmiStatus UmiCompilationDatabaseRead(const char *directory, const UmiCancellationToken *cancel,
                                     UmiCompilationDatabase **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (directory == NULL || !umi_path_is_absolute(directory))
        return UMI_STATUS_INVALID_ARGUMENT;
    char path[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = UmiOutputFileValidatePath(directory);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(directory, "compile_commands.json", path, sizeof path);
    unsigned char *bytes = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(path, UMI_COMPILATION_DATABASE_BYTE_LIMIT, &bytes, &length);
    if (status == UMI_STATUS_OK)
        status = UmiCompilationDatabaseCreate(bytes, length, cancel, out);
    UmiInputFileFree(bytes);
    return status;
}
void UmiCompilationDatabaseDestroy(UmiCompilationDatabase *database)
{
    if (database == NULL)
        return;
    UmiJsonTreeDestroy(database->tree);
    free(database->rows);
    free(database);
}
size_t UmiCompilationDatabaseCount(const UmiCompilationDatabase *database)
{
    return database == NULL ? 0U : database->count;
}
UmiStatus UmiCompilationDatabaseAt(const UmiCompilationDatabase *database, size_t index,
                                   UmiCompilationCommand *out)
{
    if (database == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= database->count)
        return UMI_STATUS_NOT_FOUND;
    return UmiCompilationDatabaseRecord(database, database->rows[index], out);
}
UmiStatus UmiCompilationDatabaseArgument(const UmiCompilationDatabase *database, size_t index,
                                         size_t argument, char *out, size_t capacity)
{
    if (database == NULL || out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= database->count)
        return UMI_STATUS_NOT_FOUND;
    int array = -1;
    UmiStatus status =
        UmiJsonTreeMember(database->tree, database->rows[index], "arguments", &array);
    if (status != UMI_STATUS_OK)
        return status;
    if (argument >= UmiJsonTreeCount(database->tree, array))
        return UMI_STATUS_NOT_FOUND;
    int node = UmiJsonTreeFirst(database->tree, array);
    for (size_t i = 0U; i < argument; ++i)
        node = UmiJsonTreeNext(database->tree, node);
    return UmiJsonTreeText(database->tree, node, out, capacity);
}
UmiStatus UmiCompilationDatabaseCommand(const UmiCompilationDatabase *database, size_t index,
                                        char *out, size_t capacity)
{
    if (database == NULL || out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= database->count)
        return UMI_STATUS_NOT_FOUND;
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(database->tree, database->rows[index], "command", &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeText(database->tree, node, out, capacity) : status;
}
UmiStatus UmiCompilationDatabaseFind(const UmiCompilationDatabase *database, const char *source,
                                     size_t start, size_t *out)
{
    if (database == NULL || out == NULL || source == NULL || !umi_path_is_absolute(source))
        return UMI_STATUS_INVALID_ARGUMENT;
    char normalised[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = umi_path_normalise(source, normalised, sizeof normalised);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(normalised);
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t index = start; index < database->count; ++index)
    {
        UmiCompilationCommand record;
        status = UmiCompilationDatabaseAt(database, index, &record);
        if (status != UMI_STATUS_OK)
            return status;
        if (umi_path_equal(record.source_file, normalised))
        {
            *out = index;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
