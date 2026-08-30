/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/application_surface_set.h
 *
 * PURPOSE:
 *   Define per-application surface registration sets and cross-application compatibility checks for Umicom Desk mosaics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_APPLICATION_SURFACE_SET_H
#define UMICOM_UI_WORKSTATION_APPLICATION_SURFACE_SET_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/surface_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsApplicationSurfaceSet {
    char application_id[UMI_UI_ID_CAPACITY];
    UmiWsApplicationDomain domain;
    char surface_ids[UMI_WS_MAX_SURFACES][UMI_UI_ID_CAPACITY];
    size_t count;
    bool allow_cross_application_hosting;
} UmiWsApplicationSurfaceSet;

UmiStatus umi_ws_application_surface_set_init(UmiWsApplicationSurfaceSet *set,
                                              const char *application_id,
                                              UmiWsApplicationDomain domain);
UmiStatus umi_ws_application_surface_set_add(UmiWsApplicationSurfaceSet *set, const char *surface_id);
bool umi_ws_application_surface_set_contains(const UmiWsApplicationSurfaceSet *set, const char *surface_id);
bool umi_ws_application_surface_set_can_host(const UmiWsApplicationSurfaceSet *host,
                                             const UmiWsSurfaceDescriptor *surface);

#ifdef __cplusplus
}
#endif

#endif
