/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/view_helpers.h
 *
 * PURPOSE:
 *   Shared toolkit-neutral UmiUiViewModel helpers for Framework Studio views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_VIEW_HELPERS_H
#define UMICOM_STUDIO_RUNTIME_VIEW_HELPERS_H

#include "umicom/ui/command_view.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the studio view create base operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_create_base(
    const char *view_id,
    const char *kind,
    const char *title,
    const char *summary,
    UmiUiViewModel **out_view);

/**
 * Provide the studio view set string operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_set_string(
    UmiUiViewModel *view,
    const char *key,
    const char *value);

/**
 * Provide the studio view set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_set_integer(
    UmiUiViewModel *view,
    const char *key,
    int64_t value);

/**
 * Provide the studio view set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_set_boolean(
    UmiUiViewModel *view,
    const char *key,
    int value);

/**
 * Provide the studio view set action operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_view_set_action(
    UmiUiViewModel *view,
    size_t index,
    const char *command_id,
    const char *label,
    const char *tooltip,
    int enabled);

#ifdef __cplusplus
}
#endif
#endif
