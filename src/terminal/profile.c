/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/profile.c
 *
 * PURPOSE:
 *   Validate and format reusable terminal profiles without implicit shell
 *   evaluation or parent-environment mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/profile.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int valid_environment_name(const char *name)
{
    size_t index;
    if (name == NULL || name[0] == '\0') return 0;
    for (index = 0U; name[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)name[index];
        if (!(isalnum(value) || value == (unsigned char)'_')) return 0;
    }
    return 1;
}

static size_t environment_index(const UmiTerminalProfile *profile,
                                const char *name)
{
    size_t index;
    for (index = 0U; index < profile->environment_count; ++index) {
        if (strcmp(profile->environment[index].name, name) == 0) return index;
    }
    return SIZE_MAX;
}

void umi_terminal_profile_init(UmiTerminalProfile *profile)
{
    if (profile == NULL) return;
    (void)memset(profile, 0, sizeof(*profile));
    profile->struct_size = (uint32_t)sizeof(*profile);
    profile->api_version = UMI_TERMINAL_PROFILE_API_VERSION;
    profile->kind = UMI_TERMINAL_PROFILE_CUSTOM;
    profile->inherit_environment = 1;
    profile->visible = 1;
}

UmiStatus umi_terminal_profile_validate(const UmiTerminalProfile *profile)
{
    size_t index;
    if (profile == NULL || profile->profile_id[0] == '\0' ||
        profile->title[0] == '\0' || profile->program[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (profile->struct_size != 0U &&
        profile->struct_size < (uint32_t)sizeof(*profile)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (profile->api_version != 0U &&
        profile->api_version != UMI_TERMINAL_PROFILE_API_VERSION) {
        return UMI_STATUS_UNAVAILABLE;
    }
    if (profile->argument_count > UMI_TERMINAL_PROFILE_ARGUMENT_MAX ||
        profile->environment_count > UMI_TERMINAL_PROFILE_ENVIRONMENT_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < profile->argument_count; ++index) {
        if (profile->argument_storage[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    for (index = 0U; index < profile->environment_count; ++index) {
        if (!valid_environment_name(profile->environment[index].name)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_profile_set_argument(UmiTerminalProfile *profile,
                                            size_t index,
                                            const char *argument)
{
    size_t length;
    if (profile == NULL || argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= UMI_TERMINAL_PROFILE_ARGUMENT_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    length = strlen(argument);
    if (length + 1U > sizeof(profile->argument_storage[index])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(profile->argument_storage[index], argument, length + 1U);
    if (profile->argument_count <= index) profile->argument_count = index + 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_profile_set_environment(UmiTerminalProfile *profile,
                                               const char *name,
                                               const char *value)
{
    size_t index;
    if (profile == NULL || !valid_environment_name(name) || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = environment_index(profile, name);
    if (index == SIZE_MAX) {
        if (profile->environment_count >= UMI_TERMINAL_PROFILE_ENVIRONMENT_MAX) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = profile->environment_count++;
    }
    if (strlen(name) + 1U > sizeof(profile->environment[index].name) ||
        strlen(value) + 1U > sizeof(profile->environment[index].value)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(profile->environment[index].name,
                   sizeof(profile->environment[index].name), "%s", name);
    (void)snprintf(profile->environment[index].value,
                   sizeof(profile->environment[index].value), "%s", value);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_profile_build_command(const UmiTerminalProfile *profile,
                                             char *out_command,
                                             size_t capacity)
{
    size_t index;
    size_t used = 0U;
    int written;
    UmiStatus status = umi_terminal_profile_validate(profile);
    if (status != UMI_STATUS_OK || out_command == NULL || capacity == 0U) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_command, capacity, "\"%s\"", profile->program);
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;
    for (index = 0U; index < profile->argument_count; ++index) {
        written = snprintf(out_command + used, capacity - used,
                           " \"%s\"", profile->argument_storage[index]);
        if (written < 0 || (size_t)written >= capacity - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return UMI_STATUS_OK;
}
