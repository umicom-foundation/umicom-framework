/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/application_runtime_policy_main.c
 *
 * PURPOSE:
 *   Let developers inspect the live behavior of a reusable panel or the
 *   runtime policy of a complete workspace recipe from the command line.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "umicom/application/presentation/presentation.h"

static void print_behavior(
    const UmiApplicationPresentationSurfaceBehavior *behavior)
{
    (void)printf("Component: %s\n", behavior->component_id);
    (void)printf("  Refresh: %s", umi_application_presentation_refresh_policy_text(
        behavior->refresh_policy));
    if (behavior->refresh_interval_seconds > 0U) {
        (void)printf(" every %u second(s)",
                     (unsigned int)behavior->refresh_interval_seconds);
    }
    (void)printf("\n  Selection: %s\n  Persistence: %s\n  Commands: %s\n",
        umi_application_presentation_selection_policy_text(
            behavior->selection_policy),
        umi_application_presentation_persistence_policy_text(
            behavior->persistence_policy),
        umi_application_presentation_command_mode_text(behavior->command_mode));
    (void)printf("  Context: %s (publishes=%s, accepts=%s)\n",
        behavior->context_group_id,
        behavior->publish_context ? "yes" : "no",
        behavior->accept_context ? "yes" : "no");
}

static void print_workspace(
    const UmiApplicationPresentationWorkspaceRuntimePolicy *policy)
{
    (void)printf("Workspace: %s\n", policy->recipe_id);
    (void)printf("  Startup: %s\n  Focus: %s\n  Background: %s\n",
        umi_application_presentation_startup_policy_text(policy->startup_policy),
        umi_application_presentation_focus_policy_text(policy->focus_policy),
        umi_application_presentation_background_policy_text(
            policy->background_policy));
    (void)printf("  Checkpoint: %s",
        umi_application_presentation_checkpoint_policy_text(
            policy->checkpoint_policy));
    if (policy->checkpoint_interval_seconds > 0U) {
        (void)printf(" every %u second(s)",
                     (unsigned int)policy->checkpoint_interval_seconds);
    }
    (void)printf("\n  Shared context: %s\n  Background commands: %s\n",
        policy->share_context ? "yes" : "no",
        policy->allow_background_commands ? "yes" : "no");
}

int main(int argc, char **argv)
{
    if (argc == 2) {
        const UmiApplicationPresentationSurfaceBehavior *behavior =
            umi_application_presentation_surface_behavior_catalogue_find(argv[1]);
        const UmiApplicationPresentationWorkspaceRuntimePolicy *workspace =
            umi_application_presentation_workspace_runtime_policy_catalogue_find(
                argv[1]);
        if (behavior != NULL) {
            print_behavior(behavior);
            return 0;
        }
        if (workspace != NULL) {
            print_workspace(workspace);
            return 0;
        }
        (void)fprintf(stderr, "No component or workspace policy named '%s'.\n",
                      argv[1]);
        return 2;
    }
    if (argc != 1) {
        (void)fprintf(stderr,
            "Usage: umicom-application-runtime-policy [component-or-recipe-id]\n");
        return 2;
    }
    (void)printf("%zu component behaviors\n",
        umi_application_presentation_surface_behavior_catalogue_count());
    (void)printf("%zu workspace runtime policies\n",
        umi_application_presentation_workspace_runtime_policy_catalogue_count());
    (void)printf("Pass an identifier to see its policy in plain language.\n");
    return 0;
}
