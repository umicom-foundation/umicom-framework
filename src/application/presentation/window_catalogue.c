/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/window_catalogue.c
 *
 * PURPOSE:
 *   Build and query portable application window specifications for every
 *   Framework-owned workspace recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/window_catalogue.h"

#include <string.h>

#include "umicom/application/component/recipe_catalogue.h"

#define WINDOW_SPEC(recipe, window, window_density, window_navigation, width, height, navigation_width, side_width, bottom_height, command_bar, status_bar, restore, multiple) \
    {sizeof(UmiApplicationPresentationWindowSpec), UMI_APPLICATION_PRESENTATION_API_VERSION,        \
     (recipe), (window), (window_density), (window_navigation), (width), (height),                   \
     (navigation_width), (side_width), (bottom_height), (command_bar), (status_bar),                 \
     (restore), (multiple)}

static const UmiApplicationPresentationWindowSpec WINDOWS[] = {
#include "window_specs/window_records.inc"
};

#undef WINDOW_SPEC

/*
 * Return the number of records represented by application presentation window catalogue
 * without changing their state.
 */
size_t umi_application_presentation_window_catalogue_count(void)
{
    return sizeof(WINDOWS) / sizeof(WINDOWS[0]);
}

/*
 * Find application presentation window catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationPresentationWindowSpec *
umi_application_presentation_window_catalogue_at(size_t index)
{
    return index < umi_application_presentation_window_catalogue_count()
        ? &WINDOWS[index]
        : NULL;
}

/*
 * Find application presentation window catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationPresentationWindowSpec *
umi_application_presentation_window_catalogue_find(const char *recipe_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (recipe_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_presentation_window_catalogue_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(WINDOWS[index].recipe_id, recipe_id) == 0) return &WINDOWS[index];
    }
    return NULL;
}

/*
 * Return the number of records represented by application presentation window catalogue
 * application without changing their state.
 */
size_t umi_application_presentation_window_catalogue_application_count(
    const char *application_id)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_presentation_window_catalogue_count(); ++index) {
        const UmiApplicationComponentRecipe *recipe =
            umi_application_component_recipe_catalogue_find(WINDOWS[index].recipe_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (recipe != NULL && strcmp(recipe->application_id, application_id) == 0) {
            count += 1U;
        }
    }
    return count;
}

/*
 * Find application presentation window catalogue application while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationPresentationWindowSpec *
umi_application_presentation_window_catalogue_application_at(
    const char *application_id,
    size_t application_index)
{
    size_t index;
    size_t match_index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_presentation_window_catalogue_count(); ++index) {
        const UmiApplicationComponentRecipe *recipe =
            umi_application_component_recipe_catalogue_find(WINDOWS[index].recipe_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (recipe == NULL || strcmp(recipe->application_id, application_id) != 0) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (match_index == application_index) return &WINDOWS[index];
        match_index += 1U;
    }
    return NULL;
}
