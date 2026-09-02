/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs_ui/view_internal.h
 *
 * PURPOSE:
 *   Declare private helpers shared by the toolkit-neutral Source Control
 *   Centre view projections.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_UI_VIEW_INTERNAL_H
#define UMICOM_VCS_UI_VIEW_INTERNAL_H
#include "umicom/ui/command_view.h"
#include "umicom/vcs_ui/vcs_ui.h"
/**
 * Provide the vcs ui base view operation used by this module and its client applications.
 */
UmiStatus umi_vcs_ui_base_view(const char *view_id, const char *kind, const char *title, const char *summary, UmiUiViewModel **out_view);
/**
 * Provide the vcs ui set string operation used by this module and its client applications.
 */
UmiStatus umi_vcs_ui_set_string(UmiUiViewModel *view, const char *key, const char *value);
/**
 * Provide the vcs ui set integer operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_ui_set_integer(UmiUiViewModel *view, const char *key, int64_t value);
/**
 * Provide the vcs ui set boolean operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_ui_set_boolean(UmiUiViewModel *view, const char *key, int value);
/**
 * Provide the vcs ui set action operation used by this module and its client applications.
 */
UmiStatus umi_vcs_ui_set_action(UmiUiViewModel *view, size_t index, const char *action_id, const char *label, const char *tooltip, int enabled);
#endif
