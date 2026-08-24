/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/closure.c
 *
 * PURPOSE:
 *   Implement complete surface/command/layout/persona closure measurement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/closure.h"

#include <string.h>

#include "umicom/ai_developer_experience/command.h"
#include "umicom/application_shell/persona.h"
#include "umicom/ide_integration/command.h"
#include "umicom/ide_integration/perspective.h"
#include "umicom/studio_runtime/command_alias_catalogue.h"
#include "umicom/studio_runtime/layout_catalogue.h"
#include "umicom/studio_runtime/runtime_commands.h"
#include "umicom/studio_runtime/surface_catalogue.h"

static UmiStatus add_missing(
    char items[UMI_STUDIO_RUNTIME_MISSING_CAPACITY]
              [UMI_STUDIO_RUNTIME_ID_CAPACITY],
    size_t *count,
    const char *value)
{
    size_t length;

    if (*count >= UMI_STUDIO_RUNTIME_MISSING_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    length = strlen(value);
    if (length >= UMI_STUDIO_RUNTIME_ID_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(items[*count], value, length + 1U);
    *count += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus check_surfaces(
    UmiApplicationShellRegistry *shell,
    UmiStudioRuntimeClosureReport *report)
{
    size_t index;

    for (index = 0U; index < umi_studio_surface_catalogue_count(); ++index) {
        const UmiStudioRuntimeSurfaceBinding *binding =
            umi_studio_surface_catalogue_at(index);
        UmiApplicationShellContribution contribution;
        UmiStatus status;

        if (binding == NULL) continue;

        status = umi_studio_runtime_surface_resolve(
            shell,
            binding,
            &contribution);

        if (status == UMI_STATUS_OK) {
            report->installed_surface_count += 1U;
        } else if (status == UMI_STATUS_NOT_FOUND) {
            status = add_missing(
                report->missing_surfaces,
                &report->missing_surface_count,
                binding->ide_surface_id);
            if (status != UMI_STATUS_OK) return status;
        } else {
            return status;
        }
    }

    return UMI_STATUS_OK;
}

static UmiStatus check_command_id(
    const UmiCommandRegistry *commands,
    const char *command_id,
    UmiStudioRuntimeClosureReport *report)
{
    if (umi_command_registry_contains(commands, command_id)) {
        return UMI_STATUS_OK;
    }

    return add_missing(
        report->missing_commands,
        &report->missing_command_count,
        command_id);
}

static UmiStatus check_commands(
    const UmiCommandRegistry *commands,
    UmiStudioRuntimeClosureReport *report)
{
    size_t index;
    UmiStatus status;

    for (index = 0U; index < umi_ide_command_count(); ++index) {
        const UmiIdeCommandDescriptor *command = umi_ide_command_at(index);

        status = check_command_id(
            commands,
            command->command_id,
            report);
        if (status != UMI_STATUS_OK) return status;

        if (umi_command_registry_contains(commands, command->command_id)) {
            report->registered_ide_command_count += 1U;
        }
    }

    for (index = 0U; index < umi_ai_developer_command_count(); ++index) {
        const UmiAiDeveloperCommandDescriptor *command =
            umi_ai_developer_command_at(index);

        status = check_command_id(
            commands,
            command->command_id,
            report);
        if (status != UMI_STATUS_OK) return status;

        if (umi_command_registry_contains(commands, command->command_id)) {
            report->registered_ai_command_count += 1U;
        }
    }

    for (index = 0U; index < umi_studio_command_alias_count(); ++index) {
        const UmiStudioRuntimeCommandAliasDefinition *alias =
            umi_studio_command_alias_at(index);

        status = check_command_id(
            commands,
            alias->alias_id,
            report);
        if (status != UMI_STATUS_OK) return status;

        if (umi_command_registry_contains(commands, alias->alias_id)) {
            report->registered_alias_count += 1U;
        }
    }

    for (index = 0U; index < umi_studio_runtime_command_count(); ++index) {
        const char *command_id = umi_studio_runtime_command_id_at(index);

        if (command_id == NULL) return UMI_STATUS_INTERNAL_ERROR;

        status = check_command_id(commands, command_id, report);
        if (status != UMI_STATUS_OK) return status;

        if (umi_command_registry_contains(commands, command_id)) {
            report->registered_studio_command_count += 1U;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_studio_runtime_closure_check(
    const UmiStudioRuntimeContract *contract,
    UmiApplicationShellRegistry *shell,
    const UmiCommandRegistry *commands,
    UmiStudioRuntimeClosureReport *out_report)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective;
    UmiStatus status;

    if (contract == NULL || shell == NULL ||
        commands == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_runtime_contract_validate(contract);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->revision = 1U;

    out_report->persona_available =
        umi_application_shell_persona_find(
            contract->shell_persona_id) != NULL;

    perspective = umi_ide_integrated_development_perspective();
    out_report->perspective_available =
        perspective != NULL &&
        strcmp(perspective->perspective_id, contract->perspective_id) == 0;

    out_report->default_layout_available =
        umi_studio_layout_catalogue_find(
            contract->default_layout_id) != NULL;
    out_report->layout_count = umi_studio_layout_catalogue_count();

    status = check_surfaces(shell, out_report);
    if (status != UMI_STATUS_OK) return status;

    status = check_commands(commands, out_report);
    if (status != UMI_STATUS_OK) return status;

    out_report->ready =
        out_report->persona_available &&
        out_report->perspective_available &&
        out_report->default_layout_available &&
        out_report->missing_surface_count == 0U &&
        out_report->missing_command_count == 0U &&
        out_report->installed_surface_count ==
            contract->required_surface_count &&
        out_report->registered_ide_command_count ==
            contract->required_ide_command_count &&
        out_report->registered_ai_command_count ==
            contract->required_ai_command_count &&
        out_report->registered_alias_count ==
            contract->required_alias_count &&
        out_report->registered_studio_command_count ==
            contract->required_studio_command_count;

    return UMI_STATUS_OK;
}
