/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/configure_definitions.c
 * PURPOSE: Keep reviewed CMake options distinct from command switches and profile-owned settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/configure_definitions.h"
#include <string.h>

static int NameStart(unsigned char value)
{
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z') || value == '_';
}
static int NamePart(unsigned char value)
{
    return NameStart(value) || (value >= '0' && value <= '9') || value == '.' || value == '-';
}
static int SpanEqual(const char *text, size_t length, const char *expected)
{
    return strlen(expected) == length && memcmp(text, expected, length) == 0;
}
static int ReservedName(const char *name, size_t length)
{
    /* These values already have one owner in the profile or generated build
     * tree. Refusing a second spelling prevents an invisible competing choice.
     * Add a name here when another dedicated setting becomes authoritative. */
    static const char *const names[] = {"CMAKE_BUILD_TYPE",    "CMAKE_C_COMPILER",
                                        "BUILD_TESTING",       "UMICOM_ENABLE_STRICT_WARNINGS",
                                        "CMAKE_GENERATOR",     "CMAKE_HOME_DIRECTORY",
                                        "CMAKE_CACHEFILE_DIR", "CMAKE_INSTALL_PREFIX"};
    for (size_t index = 0U; index < sizeof names / sizeof names[0]; ++index)
        if (SpanEqual(name, length, names[index]))
            return 1;
    return 0;
}
UmiStatus UmiBuildConfigureDefinitions(const UmiBuildProfile *profile, UmiArguments *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (profile == NULL ||
        memchr(profile->configure_definitions, '\0', sizeof profile->configure_definitions) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiArgumentsParse(profile->configure_definitions, out);
    size_t name_lengths[UMI_ARGUMENTS_CAPACITY] = {0};
    for (size_t index = 0U; status == UMI_STATUS_OK && index < out->count; ++index)
    {
        const char *token = out->values[index];
        if (strlen(token) >= UMI_BUILD_ARGUMENT_CAPACITY)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if (strncmp(token, "-D", 2U) != 0 || !NameStart((unsigned char)token[2]))
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        const char *name = token + 2U, *cursor = name;
        while (NamePart((unsigned char)*cursor))
            ++cursor;
        name_lengths[index] = (size_t)(cursor - name);
        if (ReservedName(name, name_lengths[index]))
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        if (*cursor == ':')
        {
            const char *type = ++cursor;
            while (*cursor != '\0' && *cursor != '=')
                ++cursor;
            size_t length = (size_t)(cursor - type);
            if (!SpanEqual(type, length, "BOOL") && !SpanEqual(type, length, "PATH") &&
                !SpanEqual(type, length, "FILEPATH") && !SpanEqual(type, length, "STRING") &&
                !SpanEqual(type, length, "INTERNAL"))
            {
                status = UMI_STATUS_INVALID_ARGUMENT;
                break;
            }
        }
        if (*cursor != '=')
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        /* Values remain literal argv bytes. In particular, a semicolon is a
         * CMake list separator inside this value, never a shell separator. */
        for (++cursor; *cursor != '\0'; ++cursor)
            if ((unsigned char)*cursor < 32U || (unsigned char)*cursor == 127U)
            {
                status = UMI_STATUS_INVALID_ARGUMENT;
                break;
            }
        for (size_t previous = 0U; status == UMI_STATUS_OK && previous < index; ++previous)
            if (name_lengths[previous] == name_lengths[index] &&
                memcmp(out->values[previous] + 2U, name, name_lengths[index]) == 0)
                status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (status != UMI_STATUS_OK)
        memset(out, 0, sizeof *out);
    return status;
}
