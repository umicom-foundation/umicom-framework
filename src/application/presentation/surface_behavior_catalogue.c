/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/surface_behavior_catalogue.c
 *
 * PURPOSE:
 *   Build, query and validate the governed runtime behavior catalogue for all
 *   reusable application presentation components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/surface_behavior_catalogue.h"

#include <string.h>

#include "umicom/application/presentation/panel_catalogue.h"

#define SURFACE_BEHAVIOR(component, initial, refresh, interval, selection, persistence, command, context_group, lazy, retain, publish, accept, connectivity, classification) \
    {sizeof(UmiApplicationPresentationSurfaceBehavior),                                \
     UMI_APPLICATION_PRESENTATION_BEHAVIOR_API_VERSION, (component), (initial),         \
     (refresh), (interval), (selection), (persistence), (command), (context_group),      \
     (lazy), (retain), (publish), (accept), (connectivity), (classification)}

static const UmiApplicationPresentationSurfaceBehavior BEHAVIORS[] = {
#include "behavior_profiles/behavior_records.inc"
};

#undef SURFACE_BEHAVIOR

/*
 * Return the number of records represented by application presentation surface behavior
 * catalogue without changing their state.
 */
size_t umi_application_presentation_surface_behavior_catalogue_count(void)
{
    return sizeof(BEHAVIORS) / sizeof(BEHAVIORS[0]);
}

/*
 * Find application presentation surface behavior catalogue while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationPresentationSurfaceBehavior *
umi_application_presentation_surface_behavior_catalogue_at(size_t index)
{
    return index < umi_application_presentation_surface_behavior_catalogue_count()
        ? &BEHAVIORS[index]
        : NULL;
}

/*
 * Find application presentation surface behavior catalogue while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationPresentationSurfaceBehavior *
umi_application_presentation_surface_behavior_catalogue_find(
    const char *component_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_presentation_surface_behavior_catalogue_count();
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(BEHAVIORS[index].component_id, component_id) == 0)
            return &BEHAVIORS[index];
    }
    return NULL;
}

/*
 * Return the number of records represented by application presentation surface behavior
 * catalogue refresh without changing their state.
 */
size_t umi_application_presentation_surface_behavior_catalogue_refresh_count(
    UmiApplicationPresentationRefreshPolicy policy)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_presentation_surface_behavior_catalogue_count();
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (BEHAVIORS[index].refresh_policy == policy) count += 1U;
    }
    return count;
}

/*
 * Check that application presentation surface behavior catalogue satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_application_presentation_surface_behavior_catalogue_validate(void)
{
    size_t index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_presentation_surface_behavior_catalogue_count() !=
        umi_application_presentation_panel_catalogue_count()) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_presentation_surface_behavior_catalogue_count();
         ++index) {
        size_t previous;
        UmiStatus status = umi_application_presentation_surface_behavior_validate(
            &BEHAVIORS[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Visit each bounded item once so every record receives the same rule. */
        for (previous = 0U; previous < index; ++previous) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(BEHAVIORS[previous].component_id,
                       BEHAVIORS[index].component_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}
