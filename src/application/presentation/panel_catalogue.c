/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/panel_catalogue.c
 *
 * PURPOSE:
 *   Build and query the complete independently governed reusable panel
 *   presentation catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/panel_catalogue.h"

#include <string.h>

#define PANEL_SPEC(id, icon, kind, panel_chrome, region, width, height, action, floating, closing, multiple, visible) \
    {sizeof(UmiApplicationPresentationPanelSpec), UMI_APPLICATION_PRESENTATION_API_VERSION,       \
     (id), (icon), (kind), (panel_chrome), (region), (width), (height), (action),                  \
     (floating), (closing), (multiple), (visible)}

static const UmiApplicationPresentationPanelSpec PANELS[] = {
#include "panel_specs/panel_records.inc"
};

#undef PANEL_SPEC

size_t umi_application_presentation_panel_catalogue_count(void)
{
    return sizeof(PANELS) / sizeof(PANELS[0]);
}

const UmiApplicationPresentationPanelSpec *
umi_application_presentation_panel_catalogue_at(size_t index)
{
    return index < umi_application_presentation_panel_catalogue_count()
        ? &PANELS[index]
        : NULL;
}

const UmiApplicationPresentationPanelSpec *
umi_application_presentation_panel_catalogue_find(const char *component_id)
{
    size_t index;
    if (component_id == NULL) return NULL;
    for (index = 0U; index < umi_application_presentation_panel_catalogue_count(); ++index) {
        if (strcmp(PANELS[index].component_id, component_id) == 0) {
            return &PANELS[index];
        }
    }
    return NULL;
}

size_t umi_application_presentation_panel_catalogue_kind_count(
    UmiApplicationPresentationSurfaceKind kind)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U; index < umi_application_presentation_panel_catalogue_count(); ++index) {
        if (PANELS[index].surface_kind == kind) count += 1U;
    }
    return count;
}

const UmiApplicationPresentationPanelSpec *
umi_application_presentation_panel_catalogue_kind_at(
    UmiApplicationPresentationSurfaceKind kind,
    size_t kind_index)
{
    size_t index;
    size_t match_index = 0U;
    for (index = 0U; index < umi_application_presentation_panel_catalogue_count(); ++index) {
        if (PANELS[index].surface_kind != kind) continue;
        if (match_index == kind_index) return &PANELS[index];
        match_index += 1U;
    }
    return NULL;
}
