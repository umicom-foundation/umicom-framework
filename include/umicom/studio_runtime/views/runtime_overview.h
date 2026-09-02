/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/views/runtime_overview.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral Studio Runtime view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_RUNTIME_OVERVIEW_H
#define UMICOM_STUDIO_RUNTIME_VIEW_RUNTIME_OVERVIEW_H
#include "umicom/studio_runtime/platform.h"
#include "umicom/studio_runtime/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise studio runtime overview view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_runtime_overview_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
