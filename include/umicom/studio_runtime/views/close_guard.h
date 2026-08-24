/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/views/close_guard.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral Close Safety view.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_CLOSE_GUARD_H
#define UMICOM_STUDIO_RUNTIME_VIEW_CLOSE_GUARD_H
#include "umicom/studio_runtime/platform.h"
#include "umicom/studio_runtime/close_guard.h"
#include "umicom/studio_runtime/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_close_guard_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
