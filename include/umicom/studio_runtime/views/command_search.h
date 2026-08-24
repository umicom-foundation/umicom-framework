/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/views/command_search.h
 *
 * PURPOSE:
 *   Create a toolkit-neutral command/menu search result view.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_COMMAND_SEARCH_H
#define UMICOM_STUDIO_RUNTIME_VIEW_COMMAND_SEARCH_H
#include "umicom/studio_runtime/platform.h"
#include "umicom/studio_runtime/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_studio_command_search_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    const char *query,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
