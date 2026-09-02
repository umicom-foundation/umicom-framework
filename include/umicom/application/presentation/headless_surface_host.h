/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/headless_surface_host.h
 *
 * PURPOSE:
 *   Provide a deterministic in-memory presentation host for tests, command
 *   line tools, accessibility checks and non-graphical application startup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_HEADLESS_SURFACE_HOST_H
#define UMICOM_APPLICATION_PRESENTATION_HEADLESS_SURFACE_HOST_H

#include "umicom/application/presentation/surface_host.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application presentation headless panel data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationPresentationHeadlessPanel {
    const char *component_id;
    UmiApplicationPresentationSurfaceState state;
    int visible;
    int focused;
    uint64_t revision;
} UmiApplicationPresentationHeadlessPanel;

/**
 * Represent the application presentation headless surface host data shared with callers of
 * this public contract.
 */
typedef struct UmiApplicationPresentationHeadlessSurfaceHost {
    UmiApplicationPresentationSurfaceHost host;
    UmiApplicationPresentationHeadlessPanel
        panels[UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY];
    size_t panel_count;
    const char *window_id;
    const char *focused_component_id;
    uint64_t event_count;
    int active;
} UmiApplicationPresentationHeadlessSurfaceHost;

/**
 * Initialise application presentation headless surface host from caller-provided values so
 * later operations receive a known state.
 */
void umi_application_presentation_headless_surface_host_init(
    UmiApplicationPresentationHeadlessSurfaceHost *host);
/**
 * Provide the application presentation headless surface host interface operation used by
 * this module and its client applications.
 */
UmiApplicationPresentationSurfaceHost *
umi_application_presentation_headless_surface_host_interface(
    UmiApplicationPresentationHeadlessSurfaceHost *host);
/**
 * Find application presentation headless surface host while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationPresentationHeadlessPanel *
umi_application_presentation_headless_surface_host_find(
    const UmiApplicationPresentationHeadlessSurfaceHost *host,
    const char *component_id);

#ifdef __cplusplus
}
#endif

#endif
