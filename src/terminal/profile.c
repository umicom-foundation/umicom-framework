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

/*
 * Provide the valid environment name operation used by this module and its client
 * applications.
 */
static int valid_environment_name(const char *name)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name == NULL || name[0] == '\0') return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; name[index] != '\0'; ++index) {
        unsigned char value = (unsigned char)name[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!(isalnum(value) || value == (unsigned char)'_')) return 0;
    }
    return 1;
}

/* Provide the environment index operation used by this module and its client applications. */
static size_t environment_index(const UmiTerminalProfile *profile,
                                const char *name)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->environment_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(profile->environment[index].name, name) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise terminal profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_terminal_profile_init(UmiTerminalProfile *profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) return;
    (void)memset(profile, 0, sizeof(*profile));
    profile->struct_size = (uint32_t)sizeof(*profile);
    profile->api_version = UMI_TERMINAL_PROFILE_API_VERSION;
    profile->kind = UMI_TERMINAL_PROFILE_CUSTOM;
    profile->inherit_environment = 1;
    profile->visible = 1;
}

/* Check that terminal profile satisfies its contract before another service relies on it. */
UmiStatus umi_terminal_profile_validate(const UmiTerminalProfile *profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || profile->profile_id[0] == '\0' ||
        profile->title[0] == '\0' || profile->program[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->struct_size != 0U &&
        profile->struct_size < (uint32_t)sizeof(*profile)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->api_version != 0U &&
        profile->api_version != UMI_TERMINAL_PROFILE_API_VERSION) {
        return UMI_STATUS_UNAVAILABLE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->argument_count > UMI_TERMINAL_PROFILE_ARGUMENT_MAX ||
        profile->environment_count > UMI_TERMINAL_PROFILE_ENVIRONMENT_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->argument_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (profile->argument_storage[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->environment_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!valid_environment_name(profile->environment[index].name)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the terminal profile set argument operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_profile_set_argument(UmiTerminalProfile *profile,
                                            size_t index,
                                            const char *argument)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= UMI_TERMINAL_PROFILE_ARGUMENT_MAX) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    length = strlen(argument);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > sizeof(profile->argument_storage[index])) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(profile->argument_storage[index], argument, length + 1U);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (profile->argument_count <= index) profile->argument_count = index + 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the terminal profile set environment operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_profile_set_environment(UmiTerminalProfile *profile,
                                               const char *name,
                                               const char *value)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || !valid_environment_name(name) || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = environment_index(profile, name);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (profile->environment_count >= UMI_TERMINAL_PROFILE_ENVIRONMENT_MAX) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = profile->environment_count++;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the terminal profile build command operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_profile_build_command(const UmiTerminalProfile *profile,
                                             char *out_command,
                                             size_t capacity)
{
    size_t index;
    size_t used = 0U;
    int written;
    UmiStatus status = umi_terminal_profile_validate(profile);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || out_command == NULL || capacity == 0U) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
    }
    written = snprintf(out_command, capacity, "\"%s\"", profile->program);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    used = (size_t)written;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->argument_count; ++index) {
        written = snprintf(out_command + used, capacity - used,
                           " \"%s\"", profile->argument_storage[index]);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= capacity - used) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        used += (size_t)written;
    }
    return UMI_STATUS_OK;
}
