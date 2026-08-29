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

static void print_help(void)
{
    (void)puts(
        "Usage:\n"
        "  umicom-application-presentation list [application-id]\n"
        "  umicom-application-presentation show <recipe-id>\n"
        "  umicom-application-presentation validate");
}

static int list_windows(const char *application_id)
{
    size_t index;
    for (index = 0U;
         index < umi_application_presentation_window_catalogue_count();
         ++index) {
        const UmiApplicationPresentationWindowSpec *window =
            umi_application_presentation_window_catalogue_at(index);
        const UmiApplicationComponentRecipe *recipe =
            umi_application_component_recipe_catalogue_find(window->recipe_id);
        if (recipe == NULL) {
            (void)fprintf(stderr,
                          "Window references an unknown recipe: %s\n",
                          window->recipe_id);
            return 3;
        }
        if (application_id != NULL &&
            strcmp(recipe->application_id, application_id) != 0) {
            continue;
        }
        (void)printf("%s | %s | %s | %ux%u\n",
                     window->recipe_id,
                     recipe->application_id,
                     umi_application_presentation_density_text(window->density),
                     window->initial_width,
                     window->initial_height);
    }
    return 0;
}

static int show_plan(const char *recipe_id)
{
    UmiApplicationPresentationPlan plan;
    UmiStatus status = umi_application_presentation_project(recipe_id, &plan);
    size_t index;
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
    (void)printf("Panels: %zu (%zu visible, %zu locked)\n",
                 plan.placement_count,
                 plan.visible_count,
                 plan.locked_count);
    for (index = 0U; index < plan.placement_count; ++index) {
        const UmiApplicationPresentationPanelPlacement *placement =
            &plan.placements[index];
        (void)printf("  %zu. %s | %s | %s | %s\n",
                     index + 1U,
                     placement->panel->component_id,
                     umi_application_presentation_surface_kind_text(
                         placement->panel->surface_kind),
                     umi_application_component_region_text(
                         placement->slot->region),
                     placement->slot->visible ? "visible" : "hidden");
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 1 || (argc >= 2 && strcmp(argv[1], "list") == 0)) {
        return list_windows(argc >= 3 ? argv[2] : NULL);
    }
    if (argc == 3 && strcmp(argv[1], "show") == 0) {
        return show_plan(argv[2]);
    }
    if (argc == 2 && strcmp(argv[1], "validate") == 0) {
        UmiStatus status = umi_application_presentation_catalogues_validate();
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
