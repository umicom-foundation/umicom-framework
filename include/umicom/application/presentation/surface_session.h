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

/**
 * Represent the application presentation surface session data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationPresentationSurfaceSession {
    UmiApplicationPresentationPlan plan;
    const UmiApplicationPresentationWorkspaceRuntimePolicy *workspace_policy;
    UmiApplicationPresentationSurfaceItem
        items[UMI_APPLICATION_PRESENTATION_PLAN_CAPACITY];
    size_t item_count;
    size_t focused_index;
    uint64_t revision;
} UmiApplicationPresentationSurfaceSession;

/**
 * Initialise application presentation surface session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_presentation_surface_session_init(
    const char *recipe_id,
    UmiApplicationPresentationSurfaceSession *out_session);
/**
 * Find application presentation surface session while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_find(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id);
/**
 * Provide the application presentation surface session find const operation used by this
 * module and its client applications.
 */
const UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_find_const(
    const UmiApplicationPresentationSurfaceSession *session,
    const char *component_id);
/**
 * Find application presentation surface session while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiApplicationPresentationSurfaceItem *
umi_application_presentation_surface_session_at(
    UmiApplicationPresentationSurfaceSession *session,
    size_t index);
/**
 * Perform application presentation surface session through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_application_presentation_surface_session_apply(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id,
    const UmiApplicationPresentationSurfaceUpdate *update);
/**
 * Provide the application presentation surface session set visible operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_session_set_visible(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id,
    int visible);
/**
 * Provide the application presentation surface session focus operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_presentation_surface_session_focus(
    UmiApplicationPresentationSurfaceSession *session,
    const char *component_id);
/**
 * Provide the application presentation surface session snapshot operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_presentation_surface_session_snapshot(
    const UmiApplicationPresentationSurfaceSession *session,
    UmiApplicationPresentationSurfaceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
