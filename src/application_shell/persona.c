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
            "Complete professional development shell built from reusable "
            "Framework-owned workbench services.",
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
            "Federated desktop shell for launching, docking and arranging "
            "independently built Umicom applications.",
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

/*
 * Return the number of records represented by application shell persona without changing
 * their state.
 */
size_t umi_application_shell_persona_count(void)
{
    return sizeof(PERSONAS) / sizeof(PERSONAS[0]);
}

/*
 * Find application shell persona while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiApplicationShellPersonaDefinition *
umi_application_shell_persona_at(size_t index)
{
    return index < umi_application_shell_persona_count()
        ? &PERSONAS[index]
        : NULL;
}

/*
 * Find application shell persona while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiApplicationShellPersonaDefinition *
umi_application_shell_persona_find(const char *persona_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (persona_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_shell_persona_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(PERSONAS[index].persona_id, persona_id) == 0) {
            return &PERSONAS[index];
        }
    }

    return NULL;
}

/*
 * Check that application shell persona satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_shell_persona_validate(
    const UmiApplicationShellPersonaDefinition *persona)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < persona->profile_count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_application_shell_builtin_profile_find(
                persona->profile_ids[index]) == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the application shell persona install operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_persona_install(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellPersonaDefinition *persona)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_application_shell_persona_validate(persona);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < persona->profile_count; ++index) {
        const UmiApplicationShellProfileDefinition *profile =
            umi_application_shell_builtin_profile_find(
                persona->profile_ids[index]);

        status = umi_application_shell_profile_install(registry, profile);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}
