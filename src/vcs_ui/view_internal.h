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
UmiStatus umi_vcs_ui_base_view(const char *view_id, const char *kind, const char *title, const char *summary, UmiUiViewModel **out_view);
UmiStatus umi_vcs_ui_set_string(UmiUiViewModel *view, const char *key, const char *value);
UmiStatus umi_vcs_ui_set_integer(UmiUiViewModel *view, const char *key, int64_t value);
UmiStatus umi_vcs_ui_set_boolean(UmiUiViewModel *view, const char *key, int value);
UmiStatus umi_vcs_ui_set_action(UmiUiViewModel *view, size_t index, const char *action_id, const char *label, const char *tooltip, int enabled);
#endif
