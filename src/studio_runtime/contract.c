/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/contract.c
 *
 * PURPOSE:
 *   Implement the canonical thin-Studio runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/contract.h"

#include <string.h>

#include "umicom/ai_developer_experience/command.h"
#include "umicom/application_shell/persona.h"
#include "umicom/ide_integration/command.h"
#include "umicom/ide_integration/perspective.h"
#include "umicom/studio_runtime/command_alias_catalogue.h"
#include "umicom/studio_runtime/layout_catalogue.h"
#include "umicom/studio_runtime/runtime_commands.h"
#include "umicom/studio_runtime/surface_catalogue.h"

/*
 * Provide the studio runtime contract operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeContract *umi_studio_runtime_contract(void)
{
    static const UmiStudioRuntimeContract contract = {
        .application_id = "umicom.studio",
        .title = "Umicom Studio IDE",
        .shell_persona_id = "umicom.shell.persona.studio-ide",
        .perspective_id = "umicom.perspective.integrated-development",
        .default_layout_id = "umicom.studio.layout.default",
        .required_surface_count = 26U,
        .required_ide_command_count = 29U,
        .required_ai_command_count = 23U,
        .required_alias_count = 23U,
        .required_studio_command_count = 16U,
        .required_layout_count = 9U
    };

    return &contract;
}

/*
 * Check that studio runtime contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_studio_runtime_contract_validate(
    const UmiStudioRuntimeContract *contract)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (contract == NULL ||
        contract->application_id == NULL ||
        contract->application_id[0] == '\0' ||
        contract->title == NULL ||
        contract->shell_persona_id == NULL ||
        contract->perspective_id == NULL ||
        contract->default_layout_id == NULL ||
        contract->required_surface_count !=
            umi_studio_surface_catalogue_count() ||
        contract->required_ide_command_count != umi_ide_command_count() ||
        contract->required_ai_command_count !=
            umi_ai_developer_command_count() ||
        contract->required_alias_count !=
            umi_studio_command_alias_count() ||
        contract->required_studio_command_count !=
            umi_studio_runtime_command_count() ||
        contract->required_layout_count !=
            umi_studio_layout_catalogue_count()) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_shell_persona_find(
            contract->shell_persona_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_studio_layout_catalogue_find(
            contract->default_layout_id) == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }

    perspective = umi_ide_integrated_development_perspective();
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (perspective == NULL ||
        strcmp(perspective->perspective_id, contract->perspective_id) != 0) {
        return UMI_STATUS_NOT_FOUND;
    }

    return umi_developer_workbench_perspective_validate(perspective);
}
