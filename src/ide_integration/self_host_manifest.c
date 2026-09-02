/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/self_host_manifest.c
 *
 * PURPOSE:
 *   Define self-hosting capability/command/surface requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/self_host_manifest.h"

#include <string.h>

static const char *const SURFACES[] = {
    "umicom.ide.explorer",
    "umicom.ide.editor",
    "umicom.ide.problems",
    "umicom.ide.output",
    "umicom.ide.terminal",
    "umicom.ide.source-control",
    "umicom.ide.test-explorer",
    "umicom.ide.debug",
    "umicom.ide.build",
    "umicom.ai-developer.chat",
    "umicom.ai-developer.patch-review"
};

static const char *const COMMANDS[] = {
    "ide.document.save",
    "ide.open.problem",
    "ide.open.test",
    "ide.open.symbol",
    "ide.open.debug-frame",
    "ide.inline.request",
    "ide.inline.accept",
    "ide.ai.refactor-selection",
    "ide.ai.generate-tests",
    "ide.ai.fix-problem",
    "ide.workflow.refresh",
    "ide.self-host.verify",
    "ai.developer.patch-apply",
    "developer.build",
    "developer.test",
    "source-control.commit"
};

static const char *const CAPABILITIES[] = {
    "application",
    "desktop",
    "documents",
    "editor",
    "language",
    "build",
    "testing",
    "debug",
    "vcs",
    "terminal",
    "ai",
    "security",
    "sdk",
    "abi"
};

/*
 * Provide the ide self host manifest operation used by this module and its client
 * applications.
 */
const UmiIdeSelfHostManifest *umi_ide_self_host_manifest(void)
{
    static const UmiIdeSelfHostManifest manifest = {
        .required_surface_ids = SURFACES,
        .required_surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .required_command_ids = COMMANDS,
        .required_command_count = sizeof(COMMANDS) / sizeof(COMMANDS[0]),
        .required_capabilities = CAPABILITIES,
        .required_capability_count =
            sizeof(CAPABILITIES) / sizeof(CAPABILITIES[0])
    };

    return &manifest;
}

/*
 * Check that ide self host manifest satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_ide_self_host_manifest_validate(
    const UmiIdeSelfHostManifest *manifest)
{
    size_t index;
    size_t other;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (manifest == NULL ||
        manifest->required_surface_ids == NULL ||
        manifest->required_surface_count == 0U ||
        manifest->required_command_ids == NULL ||
        manifest->required_command_count == 0U ||
        manifest->required_capabilities == NULL ||
        manifest->required_capability_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < manifest->required_surface_count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (manifest->required_surface_ids[index] == NULL ||
            manifest->required_surface_ids[index][0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U;
             other < manifest->required_surface_count;
             ++other) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(
                    manifest->required_surface_ids[index],
                    manifest->required_surface_ids[other]) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}
