/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/surface_host.h
 *
 * PURPOSE:
 *   Define the frontend boundary used to mount live application panels in
 *   GTK4, headless, web, mobile or future Umicom presentation adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_SURFACE_HOST_H
#define UMICOM_APPLICATION_PRESENTATION_SURFACE_HOST_H

#include "umicom/application/presentation/runtime_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationPresentationSurfaceHostOperations {
    uint32_t struct_size;
    UmiStatus (*begin)(void *context,
                       const UmiApplicationPresentationWindowSpec *window);
    UmiStatus (*mount)(void *context,
                       const UmiApplicationPresentationSurfaceItem *item);
    UmiStatus (*update)(void *context,
                        const UmiApplicationPresentationSurfaceItem *item);
    UmiStatus (*focus)(void *context,
                       const UmiApplicationPresentationSurfaceItem *item);
    UmiStatus (*unmount)(void *context,
                         const UmiApplicationPresentationSurfaceItem *item);
    void (*end)(void *context);
} UmiApplicationPresentationSurfaceHostOperations;

typedef struct UmiApplicationPresentationSurfaceHost {
    const UmiApplicationPresentationSurfaceHostOperations *operations;
    void *context;
} UmiApplicationPresentationSurfaceHost;

UmiStatus umi_application_presentation_surface_host_validate(
    const UmiApplicationPresentationSurfaceHost *host);

#ifdef __cplusplus
}
#endif

#endif
