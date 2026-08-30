/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/surface_session.h
 *
 * PURPOSE:
 *   Turn one projected application recipe into mutable panel visibility,
 *   focus, progress and user-feedback state without depending on a UI toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_SURFACE_SESSION_H
#define UMICOM_APPLICATION_PRESENTATION_SURFACE_SESSION_H

#include "umicom/application/presentation/projection.h"
#include "umicom/application/presentation/runtime_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationPresentationSurfaceSession {
    UmiApplicationPresentationPlan plan;
    UmiApplicationPresentationSurfaceItem
        items[UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY];
    size_t item_count;
    size_t focused_index;
    uint64_t revision;
} UmiApplicationPresentationSurfaceSession;

UmiStatus umi_application_presentation_surface_session_init(
    const char *recipe_id,
    UmiApplicationPresentationSurfaceSession *out_session);
UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_find(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id);
const UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_find_const(
    const UmiApplicationPresentationSurfaceSession *session,
    const char *component_id);
UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_at(
    UmiApplicationPresentationSurfaceSession *session,
    size_t index);
UmiStatus umi_application_presentation_surface_session_apply(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id,
    const UmiApplicationPresentationSurfaceUpdate *update);
UmiStatus umi_application_presentation_surface_session_set_visible(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id,
    int visible);
UmiStatus umi_application_presentation_surface_session_focus(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id);
UmiStatus umi_application_presentation_surface_session_snapshot(
    const UmiApplicationPresentationSurfaceSession *session,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
