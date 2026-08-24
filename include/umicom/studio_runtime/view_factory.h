/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/view_factory.h
 *
 * PURPOSE:
 *   Create Framework Studio runtime views by stable ID.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_FACTORY_H
#define UMICOM_STUDIO_RUNTIME_VIEW_FACTORY_H
#include "umicom/studio_runtime/view_ids.h"
#include "umicom/studio_runtime/views/views.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_view_factory_create(
    UmiStudioRuntimePlatform *platform,
    const char *view_id,
    const char *query,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
