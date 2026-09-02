/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_workspace_runtime_policy_catalogue.c
 *
 * PURPOSE:
 *   Verify one valid runtime policy exists for every workspace recipe and
 *   check the standard, focused and learning behaviors developers rely on.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/component/recipe_catalogue.h"
#include "umicom/application/presentation/presentation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationPresentationWorkspaceRuntimePolicy *studio;
    const UmiApplicationPresentationWorkspaceRuntimePolicy *focus;
    const UmiApplicationPresentationWorkspaceRuntimePolicy *learning;
    const UmiApplicationPresentationWorkspaceRuntimePolicy *trader;

    assert(
        umi_application_presentation_workspace_runtime_policy_catalogue_validate() ==
        UMI_STATUS_OK);
    assert(
        umi_application_presentation_workspace_runtime_policy_catalogue_count() ==
        umi_application_component_recipe_catalogue_count());

    studio =
        umi_application_presentation_workspace_runtime_policy_catalogue_find(
            "org.umicom.workspace.studio.standard");
    assert(studio != NULL);
    assert(studio->checkpoint_policy ==
           UMI_APPLICATION_PRESENTATION_CHECKPOINT_PERIODIC);
    assert(studio->checkpoint_interval_seconds == 30U);
    assert(studio->background_policy ==
           UMI_APPLICATION_PRESENTATION_BACKGROUND_REDUCED);

    focus = umi_application_presentation_workspace_runtime_policy_catalogue_find(
        "org.umicom.workspace.studio.focus");
    assert(focus != NULL);
    assert(focus->focus_policy == UMI_APPLICATION_PRESENTATION_FOCUS_PRIMARY);
    assert(focus->background_policy ==
           UMI_APPLICATION_PRESENTATION_BACKGROUND_PAUSED);
    assert(!focus->allow_background_commands);

    learning =
        umi_application_presentation_workspace_runtime_policy_catalogue_find(
            "org.umicom.workspace.studio.learning");
    assert(learning != NULL);
    assert(learning->startup_policy ==
           UMI_APPLICATION_PRESENTATION_STARTUP_ALL_VISIBLE);
    assert(learning->checkpoint_policy ==
           UMI_APPLICATION_PRESENTATION_CHECKPOINT_ON_EXIT);

    trader =
        umi_application_presentation_workspace_runtime_policy_catalogue_find(
            "org.umicom.workspace.trader.standard");
    assert(trader != NULL);
    assert(trader->checkpoint_interval_seconds == 15U);
    assert(trader->background_policy ==
           UMI_APPLICATION_PRESENTATION_BACKGROUND_FULL);
    return 0;
}
