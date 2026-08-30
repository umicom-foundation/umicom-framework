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

static size_t profile_index(const UmiTerminalProfileRegistry *registry,
                            const char *profile_id)
{
    size_t index;
    if (registry == NULL || profile_id == NULL) return SIZE_MAX;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->profiles[index].profile_id, profile_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_terminal_profile_registry_create(
    UmiTerminalProfileRegistry **out_registry)
{
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = (UmiTerminalProfileRegistry *)calloc(1U,
                                                         sizeof(**out_registry));
    return *out_registry != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_terminal_profile_registry_destroy(UmiTerminalProfileRegistry *registry)
{
    free(registry);
}

UmiStatus umi_terminal_profile_registry_register(
    UmiTerminalProfileRegistry *registry,
    const UmiTerminalProfile *profile)
{
    size_t index;
    UmiStatus status;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_terminal_profile_validate(profile);
    if (status != UMI_STATUS_OK) return status;
    index = profile_index(registry, profile->profile_id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_TERMINAL_PROFILE_MAX) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }
    registry->profiles[index] = *profile;
    registry->profiles[index].struct_size =
        (uint32_t)sizeof(registry->profiles[index]);
    registry->profiles[index].api_version = UMI_TERMINAL_PROFILE_API_VERSION;
    if (registry->default_id[0] == '\0') {
        (void)snprintf(registry->default_id, sizeof(registry->default_id),
                       "%s", profile->profile_id);
    }
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_profile_registry_remove(
    UmiTerminalProfileRegistry *registry,
    const char *profile_id)
{
    size_t index = profile_index(registry, profile_id);
    if (registry == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (strcmp(registry->default_id, profile_id) == 0) {
        registry->default_id[0] = '\0';
    }
    if (index + 1U < registry->count) {
        (void)memmove(&registry->profiles[index], &registry->profiles[index + 1U],
                      (registry->count - index - 1U) * sizeof(registry->profiles[0]));
    }
    registry->count -= 1U;
    if (registry->default_id[0] == '\0' && registry->count > 0U) {
        (void)snprintf(registry->default_id, sizeof(registry->default_id),
                       "%s", registry->profiles[0].profile_id);
    }
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_profile_registry_find(
    const UmiTerminalProfileRegistry *registry,
    const char *profile_id,
    UmiTerminalProfile *out_profile)
{
    size_t index;
    if (registry == NULL || profile_id == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = profile_index(registry, profile_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->profiles[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_profile_registry_at(
    const UmiTerminalProfileRegistry *registry,
    size_t index,
    UmiTerminalProfile *out_profile)
{
    if (registry == NULL || out_profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_profile = registry->profiles[index];
    return UMI_STATUS_OK;
}

size_t umi_terminal_profile_registry_count(
    const UmiTerminalProfileRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_terminal_profile_registry_revision(
    const UmiTerminalProfileRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

UmiStatus umi_terminal_profile_registry_set_default(
    UmiTerminalProfileRegistry *registry,
    const char *profile_id)
{
    if (registry == NULL || profile_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (profile_index(registry, profile_id) == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    (void)snprintf(registry->default_id, sizeof(registry->default_id),
                   "%s", profile_id);
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

const char *umi_terminal_profile_registry_default_id(
    const UmiTerminalProfileRegistry *registry)
{
    return registry != NULL && registry->default_id[0] != '\0'
        ? registry->default_id : NULL;
}

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

UmiStatus umi_terminal_profile_registry_add_platform_defaults(
    UmiTerminalProfileRegistry *registry)
{
    UmiStatus status;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    status = add_profile(registry, "powershell", "PowerShell",
                         UMI_TERMINAL_PROFILE_POWERSHELL, "pwsh.exe",
                         "utilities-terminal-symbolic");
    if (status == UMI_STATUS_OK) {
        status = add_profile(registry, "command-prompt", "Command Prompt",
                             UMI_TERMINAL_PROFILE_COMMAND_PROMPT, "cmd.exe",
                             "utilities-terminal-symbolic");
    }
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
    if (status == UMI_STATUS_OK) {
        status = add_profile(registry, "sh", "POSIX sh",
                             UMI_TERMINAL_PROFILE_POSIX_SH, "/bin/sh",
                             "utilities-terminal-symbolic");
    }
#endif
    return status;
}
