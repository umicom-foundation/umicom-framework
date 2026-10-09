/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/compilation_database_settings.c
 * PURPOSE: Prepare compiler-database settings as explicit drafts with no process or persistence side effects.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/configure_definitions.h"
#include "umicom/developer_project/compilation_database.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/path.h"
#include <stdlib.h>
#include <string.h>

/* A definition's value may contain colons and equals signs. Match only its
 * already validated name so an unrelated project option keeps its exact value. */
static bool CompilationExportDefinition(const char *value)
{
    const char *name = "-DCMAKE_EXPORT_COMPILE_COMMANDS";
    size_t length = strlen(name);
    return strncmp(value, name, length) == 0 && (value[length] == ':' || value[length] == '=');
}
UmiStatus UmiCompilationDatabaseEnableExport(const UmiBuildProfile *profile, UmiBuildProfile *out)
{
    if (profile == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    UmiArguments *arguments = malloc(sizeof *arguments);
    UmiBuildProfile *draft = malloc(sizeof *draft);
    if (arguments == NULL || draft == NULL)
    {
        free(arguments);
        free(draft);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *draft = *profile;
    status = UmiBuildConfigureDefinitions(profile, arguments);
    const char *values[UMI_ARGUMENTS_CAPACITY];
    size_t count = 0U;
    bool found = false;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < arguments->count; ++index)
    {
        if (CompilationExportDefinition(arguments->values[index]))
        {
            values[count++] = "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON";
            found = true;
        }
        else
            values[count++] = arguments->values[index];
    }
    if (status == UMI_STATUS_OK && !found)
    {
        if (count == UMI_ARGUMENTS_CAPACITY)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            values[count++] = "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON";
    }
    if (status == UMI_STATUS_OK)
        status = UmiArgumentsFormat(values, count, draft->configure_definitions,
                                    sizeof draft->configure_definitions);
    if (status == UMI_STATUS_OK)
        status = umi_build_profile_validate(draft, NULL, 0U);
    if (status == UMI_STATUS_OK)
        *out = *draft;
    free(draft);
    free(arguments);
    return status;
}
/* LLVM accepts one or two leading dashes. Recognise only the complete option
 * name; a similarly named custom argument must remain untouched. */
static int CompilationDirectoryOption(const char *argument)
{
    const char *name = argument;
    if (*name++ != '-')
        return 0;
    if (*name == '-')
        ++name;
    static const char option[] = "compile-commands-dir";
    size_t length = sizeof option - 1U;
    if (strncmp(name, option, length) != 0)
        return 0;
    return name[length] == '\0' ? 1 : name[length] == '=' ? 2 : 0;
}
UmiStatus UmiCompilationDatabaseClangdArguments(const char *text, const char *directory, char *out,
                                                size_t capacity)
{
    if (text == NULL || directory == NULL || out == NULL || capacity == 0U ||
        !umi_path_is_absolute(directory))
        return UMI_STATUS_INVALID_ARGUMENT;
    char normalised[UMI_BUILD_PATH_CAPACITY], selected[UMI_ARGUMENT_TEXT_CAPACITY];
    UmiStatus status = umi_path_normalise(directory, normalised, sizeof normalised);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(normalised);
    if (status != UMI_STATUS_OK)
        return status;
    static const char option[] = "--compile-commands-dir=";
    size_t length = strlen(normalised);
    if (length > sizeof selected - sizeof option)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(selected, option, sizeof option - 1U);
    memcpy(selected + sizeof option - 1U, normalised, length + 1U);
    UmiArguments *arguments = malloc(sizeof *arguments);
    /* The formatter's bounded vector can never require more than this buffer,
     * even when every byte needs quoting. Do not allocate caller-supplied sizes. */
    const size_t formatted_capacity =
        UMI_ARGUMENTS_CAPACITY * (2U * UMI_ARGUMENT_TEXT_CAPACITY + 3U) + 1U;
    char *formatted = malloc(formatted_capacity);
    if (arguments == NULL || formatted == NULL)
    {
        free(arguments);
        free(formatted);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    status = UmiArgumentsParse(text, arguments);
    const char *values[UMI_ARGUMENTS_CAPACITY];
    size_t count = 0U;
    bool found = false;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < arguments->count; ++index)
    {
        const char *value = arguments->values[index];
        if (value[0] == '@' || strcmp(value, "--") == 0)
        {
            status = UMI_STATUS_NOT_IMPLEMENTED;
            break;
        }
        int spelling = CompilationDirectoryOption(value);
        if (spelling != 0)
        {
            if (found)
            {
                status = UMI_STATUS_INVALID_ARGUMENT;
                break;
            }
            found = true;
            if (spelling == 1)
            {
                if (++index == arguments->count || arguments->values[index][0] == '\0' ||
                    arguments->values[index][0] == '-')
                {
                    status = UMI_STATUS_INVALID_ARGUMENT;
                    break;
                }
            }
            values[count++] = selected;
        }
        else
            values[count++] = value;
    }
    if (status == UMI_STATUS_OK && !found)
    {
        if (count == UMI_ARGUMENTS_CAPACITY)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            values[count++] = selected;
    }
    if (status == UMI_STATUS_OK)
        status = UmiArgumentsFormat(values, count, formatted, formatted_capacity);
    if (status == UMI_STATUS_OK && strlen(formatted) >= capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        memcpy(out, formatted, strlen(formatted) + 1U);
    free(formatted);
    free(arguments);
    return status;
}
