/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/application_presentation_main.c
 *
 * PURPOSE:
 *   Help developers list, inspect and validate reusable Framework panel and
 *   window presentation plans without starting a graphical frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/presentation.h"

#include <stdio.h>
#include <string.h>

#include "umicom/application/component/recipe_catalogue.h"
#include "umicom/application/component_catalogue.h"

/* Provide the print help operation used by this module and its client applications. */
static void print_help(void)
{
    (void)puts(
        "Usage:\n"
        "  umicom-application-presentation list [application-id]\n"
        "  umicom-application-presentation search <words>\n"
        "  umicom-application-presentation show <recipe-id>\n"
        "  umicom-application-presentation validate");
}

/* Provide the list windows operation used by this module and its client applications. */
static int list_windows(const char *application_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_presentation_window_catalogue_count();
         ++index) {
        const UmiApplicationPresentationWindowSpec *window =
            umi_application_presentation_window_catalogue_at(index);
        const UmiApplicationComponentRecipe *recipe =
            umi_application_component_recipe_catalogue_find(window->recipe_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (recipe == NULL) {
            (void)fprintf(stderr,
                          "Window references an unknown recipe: %s\n",
                          window->recipe_id);
            return 3;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (application_id != NULL &&
            strcmp(recipe->application_id, application_id) != 0) {
            continue;
        }
        (void)printf("%s | %s | %s | preferred %ux%u | minimum %ux%u\n",
                     window->recipe_id,
                     recipe->application_id,
                     umi_application_presentation_density_text(window->density),
                     window->initial_width,
                     window->initial_height,
                     window->minimum_width,
                     window->minimum_height);
    }
    return 0;
}

/* Search the canonical component catalogue and show the presentation policies
 * a frontend would apply when it creates each matching surface. */
static int search_components(const char *query)
{
    size_t index;
    size_t count = umi_application_component_search_count(query);

    (void)printf("%zu component(s) match '%s'.\n", count, query);
    for (index = 0U; index < count; ++index) {
        const UmiApplicationComponentDefinition *definition =
            umi_application_component_search_at(query, index);
        const UmiApplicationPresentationPanelSpec *panel;
        const UmiApplicationPresentationSurfaceBehavior *behavior;

        if (definition == NULL)
            return 3;
        panel = umi_application_presentation_panel_catalogue_find(
            definition->component_id);
        behavior = umi_application_presentation_surface_behavior_catalogue_find(
            definition->component_id);
        (void)printf("%s | %s | %s | compact=%s | connectivity=%s | data=%s\n",
                     definition->component_id,
                     definition->title,
                     definition->description,
                     panel != NULL
                         ? umi_application_presentation_compact_policy_text(
                               panel->compact_policy)
                         : "unknown",
                     behavior != NULL
                         ? umi_application_presentation_connectivity_text(
                               behavior->connectivity)
                         : "unknown",
                     behavior != NULL
                         ? umi_application_presentation_data_classification_text(
                               behavior->data_classification)
                         : "unknown");
    }
    return 0;
}

/* Provide the show plan operation used by this module and its client applications. */
static int show_plan(const char *recipe_id)
{
    UmiApplicationPresentationPlan plan;
    UmiStatus status = umi_application_presentation_project(recipe_id, &plan);
    size_t index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr, "Presentation plan not found: %s\n", recipe_id);
        return 2;
    }
    (void)printf("Window: %s\n", plan.window->window_id);
    (void)printf("Recipe: %s\n", plan.recipe->title);
    (void)printf("Density: %s\n",
                 umi_application_presentation_density_text(plan.window->density));
    (void)printf("Navigation: %s\n",
                 umi_application_presentation_navigation_text(
                     plan.window->navigation));
    (void)printf("Size: %ux%u\n",
                 plan.window->initial_width,
                 plan.window->initial_height);
    (void)printf("Minimum size: %ux%u\n",
                 plan.window->minimum_width,
                 plan.window->minimum_height);
    (void)printf("Panels: %zu (%zu visible, %zu locked)\n",
                 plan.placement_count,
                 plan.visible_count,
                 plan.locked_count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan.placement_count; ++index) {
        const UmiApplicationPresentationPanelPlacement *placement =
            &plan.placements[index];
        (void)printf("  %zu. %s | %s | %s | %s | compact=%s\n",
                     index + 1U,
                     placement->panel->component_id,
                     umi_application_presentation_surface_kind_text(
                         placement->panel->surface_kind),
                     umi_application_component_region_text(
                         placement->slot->region),
                     placement->slot->visible ? "visible" : "hidden",
                     umi_application_presentation_compact_policy_text(
                         placement->panel->compact_policy));
    }
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(int argc, char **argv)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (argc == 1 || (argc >= 2 && strcmp(argv[1], "list") == 0)) {
        return list_windows(argc >= 3 ? argv[2] : NULL);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (argc == 3 && strcmp(argv[1], "show") == 0) {
        return show_plan(argv[2]);
    }
    /* Search is kept separate from list so a multi-word query can address
     * component purpose without being mistaken for an application ID. */
    if (argc == 3 && strcmp(argv[1], "search") == 0) {
        return search_components(argv[2]);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (argc == 2 && strcmp(argv[1], "validate") == 0) {
        UmiStatus status = umi_application_presentation_catalogues_validate();
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            (void)fprintf(stderr,
                          "Presentation validation failed: %s\n",
                          umi_status_text(status));
            return 3;
        }
        (void)printf("Validated %zu panels and %zu windows.\n",
                     umi_application_presentation_panel_catalogue_count(),
                     umi_application_presentation_window_catalogue_count());
        return 0;
    }
    print_help();
    return 1;
}
