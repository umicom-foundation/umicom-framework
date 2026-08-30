/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/persona.c
 *
 * PURPOSE:
 *   Define reusable Studio, Desk and generic desktop shell personas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/persona.h"

#include <string.h>

static const char *const STUDIO_PROFILES[] = {
    "umicom.shell.profile.file-menu",
    "umicom.shell.profile.edit-menu",
    "umicom.shell.profile.view-menu",
    "umicom.shell.profile.navigate-menu",
    "umicom.shell.profile.build-menu",
    "umicom.shell.profile.run-menu",
    "umicom.shell.profile.test-menu",
    "umicom.shell.profile.source-control-menu",
    "umicom.shell.profile.tools-menu",
    "umicom.shell.profile.window-menu",
    "umicom.shell.profile.help-menu",
    "umicom.shell.profile.main-toolbar",
    "umicom.shell.profile.activity-bar",
    "umicom.shell.profile.status-bar",
    "umicom.shell.profile.start-center",
    "umicom.shell.profile.project-explorer",
    "umicom.shell.profile.editor",
    "umicom.shell.profile.search",
    "umicom.shell.profile.problems",
    "umicom.shell.profile.output",
    "umicom.shell.profile.terminal",
    "umicom.shell.profile.source-control",
    "umicom.shell.profile.test-explorer",
    "umicom.shell.profile.debug",
    "umicom.shell.profile.build-dashboard",
    "umicom.shell.profile.deploy-dashboard",
    "umicom.shell.profile.extensions",
    "umicom.shell.profile.settings",
    "umicom.shell.profile.ai-assistant",
    "umicom.shell.profile.project-wizard",
    "umicom.shell.profile.layout-manager",
    "umicom.shell.profile.application-launcher"
};

static const char *const DESK_PROFILES[] = {
    "umicom.shell.profile.file-menu",
    "umicom.shell.profile.view-menu",
    "umicom.shell.profile.tools-menu",
    "umicom.shell.profile.window-menu",
    "umicom.shell.profile.help-menu",
    "umicom.shell.profile.activity-bar",
    "umicom.shell.profile.status-bar",
    "umicom.shell.profile.start-center",
    "umicom.shell.profile.layout-manager",
    "umicom.shell.profile.application-launcher"
};

static const char *const GENERIC_PROFILES[] = {
    "umicom.shell.profile.file-menu",
    "umicom.shell.profile.edit-menu",
    "umicom.shell.profile.view-menu",
    "umicom.shell.profile.window-menu",
    "umicom.shell.profile.help-menu",
    "umicom.shell.profile.main-toolbar",
    "umicom.shell.profile.activity-bar",
    "umicom.shell.profile.status-bar",
    "umicom.shell.profile.start-center",
    "umicom.shell.profile.project-wizard",
    "umicom.shell.profile.layout-manager",
    "umicom.shell.profile.application-launcher"
};

static const UmiApplicationShellPersonaDefinition PERSONAS[] = {
    {
        .structure_size =
            (uint32_t)sizeof(UmiApplicationShellPersonaDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .persona_id = "umicom.shell.persona.studio-ide",
        .title = "Umicom Studio IDE",
        .description =
            "Complete IDE shell inspired by Visual Studio, IntelliJ and modern "
            "developer workbenches while remaining Framework-owned.",
        .profile_ids = STUDIO_PROFILES,
        .profile_count = sizeof(STUDIO_PROFILES) / sizeof(STUDIO_PROFILES[0])
    },
    {
        .structure_size =
            (uint32_t)sizeof(UmiApplicationShellPersonaDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .persona_id = "umicom.shell.persona.desk",
        .title = "Umicom Desk",
        .description =
            "TWS-inspired federated desktop shell for launching, docking and "
            "arranging independently built Umicom applications.",
        .profile_ids = DESK_PROFILES,
        .profile_count = sizeof(DESK_PROFILES) / sizeof(DESK_PROFILES[0])
    },
    {
        .structure_size =
            (uint32_t)sizeof(UmiApplicationShellPersonaDefinition),
        .api_version = UMI_APPLICATION_SHELL_API_VERSION,
        .persona_id = "umicom.shell.persona.generic-desktop",
        .title = "Generic Umicom Desktop Application",
        .description =
            "Reusable shell baseline for new thin-client desktop products.",
        .profile_ids = GENERIC_PROFILES,
        .profile_count = sizeof(GENERIC_PROFILES) / sizeof(GENERIC_PROFILES[0])
    }
};

size_t umi_application_shell_persona_count(void)
{
    return sizeof(PERSONAS) / sizeof(PERSONAS[0]);
}

const UmiApplicationShellPersonaDefinition *
umi_application_shell_persona_at(size_t index)
{
    return index < umi_application_shell_persona_count()
        ? &PERSONAS[index]
        : NULL;
}

const UmiApplicationShellPersonaDefinition *
umi_application_shell_persona_find(const char *persona_id)
{
    size_t index;

    if (persona_id == NULL) return NULL;

    for (index = 0U; index < umi_application_shell_persona_count(); ++index) {
        if (strcmp(PERSONAS[index].persona_id, persona_id) == 0) {
            return &PERSONAS[index];
        }
    }

    return NULL;
}

UmiStatus umi_application_shell_persona_validate(
    const UmiApplicationShellPersonaDefinition *persona)
{
    size_t index;

    if (persona == NULL ||
        persona->structure_size != sizeof(*persona) ||
        persona->api_version != UMI_APPLICATION_SHELL_API_VERSION ||
        persona->persona_id == NULL || persona->persona_id[0] == '\0' ||
        persona->title == NULL || persona->title[0] == '\0' ||
        persona->description == NULL ||
        persona->profile_ids == NULL ||
        persona->profile_count == 0U ||
        persona->profile_count > UMI_APPLICATION_SHELL_PERSONA_MAX_PROFILES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < persona->profile_count; ++index) {
        if (umi_application_shell_builtin_profile_find(
                persona->profile_ids[index]) == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_application_shell_persona_install(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellPersonaDefinition *persona)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_application_shell_persona_validate(persona);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < persona->profile_count; ++index) {
        const UmiApplicationShellProfileDefinition *profile =
            umi_application_shell_builtin_profile_find(
                persona->profile_ids[index]);

        status = umi_application_shell_profile_install(registry, profile);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
