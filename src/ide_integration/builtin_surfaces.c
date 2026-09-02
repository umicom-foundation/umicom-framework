/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/builtin_surfaces.c
 *
 * PURPOSE:
 *   Register the complete reusable IDE surface family.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/builtin_surfaces.h"

static const UmiIdeSurfaceProfileFactory FACTORIES[] = {
    umi_ide_surface_profile_explorer,
    umi_ide_surface_profile_editor,
    umi_ide_surface_profile_outline,
    umi_ide_surface_profile_search,
    umi_ide_surface_profile_problems,
    umi_ide_surface_profile_output,
    umi_ide_surface_profile_terminal,
    umi_ide_surface_profile_source_control,
    umi_ide_surface_profile_test_explorer,
    umi_ide_surface_profile_debug,
    umi_ide_surface_profile_build,
    umi_ide_surface_profile_ai_chat,
    umi_ide_surface_profile_ai_review,
    umi_ide_surface_profile_ai_approvals,
    umi_ide_surface_profile_ai_tools,
    umi_ide_surface_profile_ai_validation
};

/*
 * Provide the ide builtin surfaces install operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_builtin_surfaces_install(UmiIdeSurfaceRegistry *registry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(FACTORIES) / sizeof(FACTORIES[0]); ++index) {
        UmiStatus status = umi_ide_surface_registry_add(
            registry,
            FACTORIES[index]());

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK &&
            status != UMI_STATUS_ALREADY_EXISTS) {
            return status;
        }
    }

    return UMI_STATUS_OK;
}
