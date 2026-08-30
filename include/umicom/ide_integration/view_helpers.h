/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/view_helpers.h
 *
 * PURPOSE:
 *   Shared toolkit-neutral view-model helpers for IDE integration panes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_VIEW_HELPERS_H
#define UMICOM_IDE_INTEGRATION_VIEW_HELPERS_H

#include "umicom/ui/command_view.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ide_view_create_base(
    const char *view_id,
    const char *kind,
    const char *title,
    const char *summary,
    UmiUiViewModel **out_view);

UmiStatus umi_ide_view_set_string(
    UmiUiViewModel *view,
    const char *key,
    const char *text);

UmiStatus umi_ide_view_set_integer(
    UmiUiViewModel *view,
    const char *key,
    int64_t value);

UmiStatus umi_ide_view_set_boolean(
    UmiUiViewModel *view,
    const char *key,
    int value);

UmiStatus umi_ide_view_set_action(
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
