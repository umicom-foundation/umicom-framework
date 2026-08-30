/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/surface_behavior_catalogue.h
 *
 * PURPOSE:
 *   Discover the Framework-owned live behavior profile for every reusable
 *   application component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_SURFACE_BEHAVIOR_CATALOGUE_H
#define UMICOM_APPLICATION_PRESENTATION_SURFACE_BEHAVIOR_CATALOGUE_H

#include "umicom/application/presentation/surface_behavior.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_application_presentation_surface_behavior_catalogue_count(void);
const UmiApplicationPresentationSurfaceBehavior *
umi_application_presentation_surface_behavior_catalogue_at(size_t index);
const UmiApplicationPresentationSurfaceBehavior *
umi_application_presentation_surface_behavior_catalogue_find(
    const char *component_id);
size_t umi_application_presentation_surface_behavior_catalogue_refresh_count(
    UmiApplicationPresentationRefreshPolicy policy);
UmiStatus umi_application_presentation_surface_behavior_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
