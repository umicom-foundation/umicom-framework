/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/profile_registry.c
 *
 * PURPOSE:
 *   Implement bounded terminal profile registration and platform defaults.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/profile_registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiTerminalProfileRegistry {
    UmiTerminalProfile profiles[UMI_TERMINAL_PROFILE_MAX];
    size_t count;
    char default_id[UMI_TERMINAL_ID_CAPACITY];
    uint64_t revision;
};

/* Provide the profile index operation used by this module and its client applications. */
static size_t profile_index(const UmiTerminalProfileRegistry *registry,
                            const char *profile_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->profiles[index].profile_id, profile_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Initialise terminal profile registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_terminal_profile_registry_create(
    UmiTerminalProfileRegistry **out_registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = (UmiTerminalProfileRegistry *)calloc(1U,
                                                         sizeof(**out_registry));
    return *out_registry != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/*
 * Release or reset state held by terminal profile registry so the same storage can be
 * reused safely.
 */
void umi_terminal_profile_registry_destroy(UmiTerminalProfileRegistry *registry)
{
    free(registry);
}

/*
 * Add terminal profile registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_terminal_profile_registry_register(
    UmiTerminalProfileRegistry *registry,
    const UmiTerminalProfile *profile)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_terminal_profile_validate(profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    index = profile_index(registry, profile->profile_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_TERMINAL_PROFILE_MAX) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }
    registry->profiles[index] = *profile;
    registry->profiles[index].struct_size =
        (uint32_t)sizeof(registry->profiles[index]);
    registry->profiles[index].api_version = UMI_TERMINAL_PROFILE_API_VERSION;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (registry->default_id[0] == '\0') {
        (void)snprintf(registry->default_id, sizeof(registry->default_id),
                       "%s", profile->profile_id);
    }
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Remove terminal profile registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_terminal_profile_registry_remove(
    UmiTerminalProfileRegistry *registry,
    const char *profile_id)
{
    size_t index = profile_index(registry, profile_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(registry->default_id, profile_id) == 0) {
        registry->default_id[0] = '\0';
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        (void)memmove(&registry->profiles[index], &registry->profiles[index + 1U],
                      (registry->count - index - 1U) * sizeof(registry->profiles[0]));
    }
    registry->count -= 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->default_id[0] == '\0' && registry->count > 0U) {
        (void)snprintf(registry->default_id, sizeof(registry->default_id),
                       "%s", registry->profiles[0].profile_id);
    }
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find terminal profile registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_terminal_profile_registry_find(
    const UmiTerminalProfileRegistry *registry,
    const char *profile_id,
    UmiTerminalProfile *out_profile)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile_id == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = profile_index(registry, profile_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->profiles[index];
    return UMI_STATUS_OK;
}

/*
 * Find terminal profile registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_terminal_profile_registry_at(
    const UmiTerminalProfileRegistry *registry,
    size_t index,
    UmiTerminalProfile *out_profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->profiles[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by terminal profile registry without changing
 * their state.
 */
size_t umi_terminal_profile_registry_count(
    const UmiTerminalProfileRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the terminal profile registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_profile_registry_revision(
    const UmiTerminalProfileRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/*
 * Provide the terminal profile registry set default operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_profile_registry_set_default(
    UmiTerminalProfileRegistry *registry,
    const char *profile_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (profile_index(registry, profile_id) == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    (void)snprintf(registry->default_id, sizeof(registry->default_id),
                   "%s", profile_id);
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the terminal profile registry default id operation used by this module and its
 * client applications.
 */
const char *umi_terminal_profile_registry_default_id(
    const UmiTerminalProfileRegistry *registry)
{
    return registry != NULL && registry->default_id[0] != '\0'
        ? registry->default_id : NULL;
}

/* Provide the add profile operation used by this module and its client applications. */
static UmiStatus add_profile(UmiTerminalProfileRegistry *registry,
                             const char *id,
                             const char *title,
                             UmiTerminalProfileKind kind,
                             const char *program,
                             const char *icon)
{
    UmiTerminalProfile profile;
    umi_terminal_profile_init(&profile);
    (void)snprintf(profile.profile_id, sizeof(profile.profile_id), "%s", id);
    (void)snprintf(profile.title, sizeof(profile.title), "%s", title);
    (void)snprintf(profile.program, sizeof(profile.program), "%s", program);
    (void)snprintf(profile.icon_name, sizeof(profile.icon_name), "%s", icon);
    profile.kind = kind;
    return umi_terminal_profile_registry_register(registry, &profile);
}

/*
 * Provide the terminal profile registry add platform defaults operation used by this
 * module and its client applications.
 */
UmiStatus umi_terminal_profile_registry_add_platform_defaults(
    UmiTerminalProfileRegistry *registry)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    status = add_profile(registry, "powershell", "PowerShell",
                         UMI_TERMINAL_PROFILE_POWERSHELL, "pwsh.exe",
                         "utilities-terminal-symbolic");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_profile(registry, "command-prompt", "Command Prompt",
                             UMI_TERMINAL_PROFILE_COMMAND_PROMPT, "cmd.exe",
                             "utilities-terminal-symbolic");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_profile(registry, "msys2-ucrt64", "MSYS2 UCRT64",
                             UMI_TERMINAL_PROFILE_MSYS2_UCRT64,
                             "C:/msys64/usr/bin/bash.exe",
                             "utilities-terminal-symbolic");
    }
#else
    status = add_profile(registry, "bash", "Bash",
                         UMI_TERMINAL_PROFILE_BASH, "/bin/bash",
                         "utilities-terminal-symbolic");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = add_profile(registry, "sh", "POSIX sh",
                             UMI_TERMINAL_PROFILE_POSIX_SH, "/bin/sh",
                             "utilities-terminal-symbolic");
    }
#endif
    return status;
}
