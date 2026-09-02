/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/view_factory.h
 *
 * PURPOSE:
 *   Create Framework Studio runtime views by stable ID.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_FACTORY_H
#define UMICOM_STUDIO_RUNTIME_VIEW_FACTORY_H
#include "umicom/studio_runtime/view_ids.h"
#include "umicom/studio_runtime/views/views.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise studio view factory from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_studio_view_factory_create(
    UmiStudioRuntimePlatform *platform,
    const char *view_id,
    const char *query,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
