/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/panel_catalogue.h
 *
 * PURPOSE:
 *   Expose the complete catalogue of frontend-neutral presentation contracts
 *   for Framework-owned application components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_PANEL_CATALOGUE_H
#define UMICOM_APPLICATION_PRESENTATION_PANEL_CATALOGUE_H

#include "umicom/application/presentation/panel_spec.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_presentation_panel_catalogue_count(void);
const UmiApplicationPresentationPanelSpec *
umi_application_presentation_panel_catalogue_at(size_t index);
const UmiApplicationPresentationPanelSpec *
umi_application_presentation_panel_catalogue_find(const char *component_id);
size_t umi_application_presentation_panel_catalogue_kind_count(
    UmiApplicationPresentationSurfaceKind kind);
const UmiApplicationPresentationPanelSpec *
umi_application_presentation_panel_catalogue_kind_at(
    UmiApplicationPresentationSurfaceKind kind,
    size_t kind_index);

#ifdef __cplusplus
}
#endif

#endif
