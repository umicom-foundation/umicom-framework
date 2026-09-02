/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/views/layout_browser.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral Layout Browser view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_LAYOUT_BROWSER_H
#define UMICOM_STUDIO_RUNTIME_VIEW_LAYOUT_BROWSER_H
#include "umicom/studio_runtime/platform.h"
#include "umicom/studio_runtime/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Initialise studio layout browser view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_layout_browser_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
