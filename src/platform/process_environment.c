/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_environment.c
 * PURPOSE: Share bounded launch-environment parsing between build, debugger and persistent tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/process_environment.h"
#include "umicom/base/arguments.h"
#include "umicom/platform/process_search_path.h"
#include <stdlib.h>
#include <string.h>
struct UmiProcessEnvironmentPlan
{
    char names[UMI_PROCESS_ENVIRONMENT_ENTRY_CAPACITY + 1U][64];
    char values[UMI_PROCESS_ENVIRONMENT_ENTRY_CAPACITY + 1U][UMI_PROCESS_ENVIRONMENT_TEXT_CAPACITY];
    UmiEnvironmentVariable variables[UMI_PROCESS_ENVIRONMENT_ENTRY_CAPACITY + 1U];
    size_t count;
    char *search_path;
};
static int EnvironmentLetter(unsigned char byte)
{
    return (byte >= 'A' && byte <= 'Z') || (byte >= 'a' && byte <= 'z') || byte == '_';
}
static unsigned char EnvironmentFold(unsigned char byte)
{
    return byte >= 'a' && byte <= 'z' ? (unsigned char)(byte - 'a' + 'A') : byte;
}
static int EnvironmentNameEqual(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0')
    {
        if (EnvironmentFold((unsigned char)*left++) != EnvironmentFold((unsigned char)*right++))
            return 0;
    }
    return *left == *right;
}
/* Validate UTF-8 before a profile can cross between native platforms. In
 * particular, reject overlong sequences, surrogates and out-of-range codepoints
 * rather than letting Windows reject settings previously accepted on POSIX. */
static int EnvironmentValueValid(const char *text)
{
    const unsigned char *at = (const unsigned char *)text;
    while (*at != 0U)
    {
        unsigned char first = *at++;
        if (first < 32U || first == 127U)
            return 0;
        if (first < 128U)
            continue;
        unsigned count;
        uint32_t point, minimum;
        if (first >= 0xc2U && first <= 0xdfU)
        {
            count = 1U;
            point = first & 0x1fU;
            minimum = 0x80U;
        }
        else if (first >= 0xe0U && first <= 0xefU)
        {
            count = 2U;
            point = first & 0x0fU;
            minimum = 0x800U;
        }
        else if (first >= 0xf0U && first <= 0xf4U)
        {
            count = 3U;
            point = first & 0x07U;
            minimum = 0x10000U;
        }
        else
            return 0;
        for (unsigned index = 0U; index < count; ++index)
        {
            if ((*at & 0xc0U) != 0x80U)
                return 0;
            point = (point << 6U) | (*at++ & 0x3fU);
        }
        if (point < minimum || point > 0x10ffffU || (point >= 0xd800U && point <= 0xdfffU))
            return 0;
    }
    return 1;
}
static UmiStatus EnvironmentParse(const char *definitions, UmiProcessEnvironmentPlan *plan)
{
    if (definitions == NULL)
        definitions = "";
    size_t length = 0U;
    while (length < UMI_PROCESS_ENVIRONMENT_TEXT_CAPACITY && definitions[length] != '\0')
        ++length;
    if (length == UMI_PROCESS_ENVIRONMENT_TEXT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Syntax parsing uses owned temporary storage. Publish pointers only into
     * the plan, so copying a temporary argument record cannot leave dangling values. */
    UmiArguments *arguments = calloc(1U, sizeof *arguments);
    if (arguments == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiArgumentsParse(definitions, arguments);
    if (status == UMI_STATUS_OK && arguments->count > UMI_PROCESS_ENVIRONMENT_ENTRY_CAPACITY)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < arguments->count; ++index)
    {
        const char *assignment = arguments->values[index];
        const char *equals = strchr(assignment, '=');
        if (equals == NULL || equals == assignment ||
            !EnvironmentLetter((unsigned char)assignment[0]))
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        size_t name_length = (size_t)(equals - assignment);
        if (name_length >= sizeof plan->names[index])
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        for (size_t letter = 1U; letter < name_length; ++letter)
        {
            unsigned char byte = (unsigned char)assignment[letter];
            if (!EnvironmentLetter(byte) && !(byte >= '0' && byte <= '9'))
            {
                status = UMI_STATUS_INVALID_ARGUMENT;
                break;
            }
        }
        if (status != UMI_STATUS_OK)
            break;
        if (!EnvironmentValueValid(equals + 1U))
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        memcpy(plan->names[index], assignment, name_length);
        plan->names[index][name_length] = '\0';
        if (EnvironmentNameEqual(plan->names[index], "PATH"))
            strcpy(plan->names[index], "PATH");
        for (size_t previous = 0U; previous < index; ++previous)
            if (EnvironmentNameEqual(plan->names[previous], plan->names[index]))
                status = UMI_STATUS_ALREADY_EXISTS;
        if (status != UMI_STATUS_OK)
            break;
        memcpy(plan->values[index], equals + 1U, strlen(equals + 1U) + 1U);
        plan->variables[index].name = plan->names[index];
        plan->variables[index].value = plan->values[index];
        ++plan->count;
    }
    free(arguments);
    return status;
}
UmiStatus UmiProcessEnvironmentValidate(const char *definitions)
{
    UmiProcessEnvironmentPlan *plan = calloc(1U, sizeof *plan);
    if (plan == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = EnvironmentParse(definitions, plan);
    free(plan);
    return status;
}
UmiStatus UmiProcessEnvironmentPlanCreate(const char *definitions, const char *tool_directory,
                                          UmiProcessEnvironmentPlan **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiProcessEnvironmentPlan *plan = calloc(1U, sizeof *plan);
    if (plan == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = EnvironmentParse(definitions, plan);
    if (status == UMI_STATUS_OK && tool_directory != NULL && tool_directory[0] != '\0')
    {
        size_t path_index = plan->count;
        for (size_t index = 0U; index < plan->count; ++index)
            if (strcmp(plan->names[index], "PATH") == 0)
                path_index = index;
        status = path_index < plan->count
                     ? UmiProcessSearchPathJoin(tool_directory, plan->values[path_index],
                                                &plan->search_path)
                     : UmiProcessSearchPathCapture(tool_directory, &plan->search_path);
        if (status == UMI_STATUS_OK)
        {
            if (path_index == plan->count)
            {
                strcpy(plan->names[path_index], "PATH");
                ++plan->count;
            }
            plan->variables[path_index].name = plan->names[path_index];
            plan->variables[path_index].value = plan->search_path;
        }
    }
    if (status != UMI_STATUS_OK)
    {
        UmiProcessEnvironmentPlanDestroy(plan);
        return status;
    }
    *out = plan;
    return UMI_STATUS_OK;
}
UmiStatus UmiProcessEnvironmentPlanRead(const UmiProcessEnvironmentPlan *plan,
                                        const UmiEnvironmentVariable **out_variables,
                                        size_t *out_count)
{
    if (out_variables != NULL)
        *out_variables = NULL;
    if (out_count != NULL)
        *out_count = 0U;
    if (plan == NULL || out_variables == NULL || out_count == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_variables = plan->variables;
    *out_count = plan->count;
    return UMI_STATUS_OK;
}
void UmiProcessEnvironmentPlanDestroy(UmiProcessEnvironmentPlan *plan)
{
    if (plan == NULL)
        return;
    UmiProcessSearchPathFree(plan->search_path);
    free(plan);
}
