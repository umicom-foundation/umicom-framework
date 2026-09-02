/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/quick_access.h
 *
 * PURPOSE:
 *   Search the canonical command registry for command-palette and quick-access surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The search engine belongs to Framework; a frontend only renders ranked results and asks the command registry to execute the selected command.
 */

#ifndef UMICOM_UI_QUICK_ACCESS_H
#define UMICOM_UI_QUICK_ACCESS_H

#include <stddef.h>
#include "umicom/runtime/command_registry.h"
#include "umicom/ui/action.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_QUICK_ACCESS_MAX_RESULTS 64U
#define UMI_UI_QUICK_ACCESS_REQUEST_CONTEXT_KEY \
    "umicom.ui.quick-access.request"

/**
 * Represent the ui quick access item data shared with callers of this public contract.
 */
typedef struct UmiUiQuickAccessItem {
    char action_id[UMI_UI_ID_CAPACITY];
    char command_id[UMI_COMMAND_ID_CAPACITY];
    char title[UMI_COMMAND_TITLE_CAPACITY];
    char category[UMI_COMMAND_CATEGORY_CAPACITY];
    char description[UMI_COMMAND_DESCRIPTION_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    char accelerator[UMI_UI_ACCELERATOR_CAPACITY];
    int score;
    int enabled;
    int checkable;
    int checked;
} UmiUiQuickAccessItem;

/**
 * Represent the ui quick access results data shared with callers of this public contract.
 */
typedef struct UmiUiQuickAccessResults {
    UmiUiQuickAccessItem items[UMI_UI_QUICK_ACCESS_MAX_RESULTS];
    size_t count;
} UmiUiQuickAccessResults;

/**
 * Provide the ui quick access score operation used by this module and its client
 * applications.
 */
int umi_ui_quick_access_score(const char *query, const char *candidate);
/**
 * Provide the ui quick access search commands operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_quick_access_search_commands(
    const UmiCommandRegistry *registry,
    const char *query,
    UmiUiQuickAccessResults *out_results);
/**
 * Provide the ui quick access search actions operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_quick_access_search_actions(
    const UmiCommandRegistry *registry,
    const UmiUiActionModel *actions,
    const char *query,
    UmiUiQuickAccessResults *out_results);

#ifdef __cplusplus
}
#endif
#endif
