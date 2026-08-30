/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/surface_checkpoint.h
 *
 * PURPOSE:
 *   Capture and restore portable panel visibility, focus and dirty-state data
 *   without serialising toolkit widgets or product service objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_SURFACE_CHECKPOINT_H
#define UMICOM_APPLICATION_PRESENTATION_SURFACE_CHECKPOINT_H

#include "umicom/application/presentation/surface_session.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationPresentationSurfaceCheckpointItem {
    const char *component_id;
    int visible;
    int focused;
    int dirty;
} UmiApplicationPresentationSurfaceCheckpointItem;

typedef struct UmiApplicationPresentationSurfaceCheckpoint {
    const char *recipe_id;
    UmiApplicationPresentationSurfaceCheckpointItem
        items[UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY];
    size_t item_count;
    uint64_t source_revision;
} UmiApplicationPresentationSurfaceCheckpoint;

UmiStatus umi_application_presentation_surface_checkpoint_capture(
    const UmiApplicationPresentationSurfaceSession *session,
    UmiApplicationPresentationSurfaceCheckpoint *out_checkpoint);
UmiStatus umi_application_presentation_surface_checkpoint_restore(
    UmiApplicationPresentationSurfaceSession *session,
    const UmiApplicationPresentationSurfaceCheckpoint *checkpoint);

#ifdef __cplusplus
}
#endif

#endif
