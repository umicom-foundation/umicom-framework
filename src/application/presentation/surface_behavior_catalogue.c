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

#define SURFACE_BEHAVIOR(component, initial, refresh, interval, selection, persistence, command, context_group, lazy, retain, publish, accept) \
    {sizeof(UmiApplicationPresentationSurfaceBehavior),                                \
     UMI_APPLICATION_PRESENTATION_BEHAVIOR_API_VERSION, (component), (initial),         \
     (refresh), (interval), (selection), (persistence), (command), (context_group),      \
     (lazy), (retain), (publish), (accept)}

static const UmiApplicationPresentationSurfaceBehavior BEHAVIORS[] = {
#include "behavior_profiles/behavior_records.inc"
};

#undef SURFACE_BEHAVIOR

size_t umi_application_presentation_surface_behavior_catalogue_count(void)
{
    return sizeof(BEHAVIORS) / sizeof(BEHAVIORS[0]);
}

const UmiApplicationPresentationSurfaceBehavior *
umi_application_presentation_surface_behavior_catalogue_at(size_t index)
{
    return index < umi_application_presentation_surface_behavior_catalogue_count()
        ? &BEHAVIORS[index]
        : NULL;
}

const UmiApplicationPresentationSurfaceBehavior *
umi_application_presentation_surface_behavior_catalogue_find(
    const char *component_id)
{
    size_t index;
    if (component_id == NULL) return NULL;
    for (index = 0U;
         index < umi_application_presentation_surface_behavior_catalogue_count();
         ++index) {
        if (strcmp(BEHAVIORS[index].component_id, component_id) == 0)
            return &BEHAVIORS[index];
    }
    return NULL;
}

size_t umi_application_presentation_surface_behavior_catalogue_refresh_count(
    UmiApplicationPresentationRefreshPolicy policy)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U;
         index < umi_application_presentation_surface_behavior_catalogue_count();
         ++index) {
        if (BEHAVIORS[index].refresh_policy == policy) count += 1U;
    }
    return count;
}

UmiStatus umi_application_presentation_surface_behavior_catalogue_validate(void)
{
    size_t index;
    if (umi_application_presentation_surface_behavior_catalogue_count() !=
        umi_application_presentation_panel_catalogue_count()) {
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U;
         index < umi_application_presentation_surface_behavior_catalogue_count();
         ++index) {
        size_t previous;
        UmiStatus status = umi_application_presentation_surface_behavior_validate(
            &BEHAVIORS[index]);
        if (status != UMI_STATUS_OK) return status;
        for (previous = 0U; previous < index; ++previous) {
            if (strcmp(BEHAVIORS[previous].component_id,
                       BEHAVIORS[index].component_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}
