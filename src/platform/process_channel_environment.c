/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_channel_environment.c
 * PURPOSE: Validate explicit environment overrides for trusted interactive programs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "process_channel_internal.h"

static int ChannelNameLetter(unsigned char value)
{
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z') || value == '_';
}
static unsigned char ChannelNameFold(unsigned char value)
{
    return value >= 'a' && value <= 'z' ? (unsigned char)(value - 'a' + 'A') : value;
}
/* Reject malformed UTF-8 before Windows conversion or POSIX byte copying. */
static int ChannelUtf8(const unsigned char *text)
{
    while (*text != 0U)
    {
        unsigned char first = *text++;
        if (first < 128U)
            continue;
        unsigned count;
        uint32_t code, minimum;
        if (first >= 0xc2U && first <= 0xdfU)
        {
            count = 1U;
            code = first & 31U;
            minimum = 0x80U;
        }
        else if (first >= 0xe0U && first <= 0xefU)
        {
            count = 2U;
            code = first & 15U;
            minimum = 0x800U;
        }
        else if (first >= 0xf0U && first <= 0xf4U)
        {
            count = 3U;
            code = first & 7U;
            minimum = 0x10000U;
        }
        else
            return 0;
        for (unsigned index = 0U; index < count; ++index)
        {
            if ((*text & 0xc0U) != 0x80U)
                return 0;
            code = (code << 6U) | (*text++ & 63U);
        }
        if (code < minimum || code > 0x10ffffU || (code >= 0xd800U && code <= 0xdfffU))
            return 0;
    }
    return 1;
}
UmiStatus PcEnvironmentValidate(const UmiEnvironmentVariable *environment, size_t count)
{
    if (count > UMI_PROCESS_MAX_ENVIRONMENT || (count != 0U && environment == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < count; ++index)
    {
        const char *name = environment[index].name, *value = environment[index].value;
        if (name == NULL || value == NULL || !ChannelNameLetter((unsigned char)name[0]))
            return UMI_STATUS_INVALID_ARGUMENT;
        size_t length = strlen(name);
        if (length > 63U || strlen(value) > 131068U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t at = 1U; at < length; ++at)
            if (!ChannelNameLetter((unsigned char)name[at]) &&
                !(name[at] >= '0' && name[at] <= '9'))
                return UMI_STATUS_INVALID_ARGUMENT;
        if (!ChannelUtf8((const unsigned char *)value))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t previous = 0U; previous < index; ++previous)
        {
            const char *other = environment[previous].name;
            if (strlen(other) != length)
                continue;
            size_t same = 0U;
            while (same < length && ChannelNameFold((unsigned char)other[same]) ==
                                        ChannelNameFold((unsigned char)name[same]))
                ++same;
            if (same == length)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    return UMI_STATUS_OK;
}
