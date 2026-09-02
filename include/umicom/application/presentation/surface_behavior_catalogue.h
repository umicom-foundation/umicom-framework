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

/**
 * Return the number of records represented by application presentation surface behavior
 * catalogue without changing their state.
 */
size_t umi_application_presentation_surface_behavior_catalogue_count(void);
/**
 * Find application presentation surface behavior catalogue while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationPresentationSurfaceBehavior *
umi_application_presentation_surface_behavior_catalogue_at(size_t index);
/**
 * Find application presentation surface behavior catalogue while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationPresentationSurfaceBehavior *
umi_application_presentation_surface_behavior_catalogue_find(
    const char *component_id);
/**
 * Return the number of records represented by application presentation surface behavior
 * catalogue refresh without changing their state.
 */
size_t umi_application_presentation_surface_behavior_catalogue_refresh_count(
    UmiApplicationPresentationRefreshPolicy policy);
/**
 * Check that application presentation surface behavior catalogue satisfies its contract
 * before another service relies on it.
 */
UmiStatus umi_application_presentation_surface_behavior_catalogue_validate(void);

#ifdef __cplusplus
}
#endif

#endif
