/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/navigation_history.c
 *
 * PURPOSE:
 *   Implement bounded Back/Forward navigation history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/navigation_history.h"

#include <string.h>

/*
 * Initialise developer navigation history from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_navigation_history_init(
    UmiDeveloperNavigationHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    (void)memset(history, 0, sizeof(*history));
    history->revision = 1U;
}

/*
 * Provide the developer navigation history push operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_navigation_history_push(
    UmiDeveloperNavigationHistory *history,
    const UmiDeveloperProductivityLocation *location)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_productivity_location_validate(location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (history->has_cursor &&
        history->cursor + 1U < history->count) {
        history->count = history->cursor + 1U;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count >= UMI_DEVELOPER_NAVIGATION_HISTORY_CAPACITY) {
        (void)memmove(
            &history->locations[0],
            &history->locations[1],
            (history->count - 1U) * sizeof(history->locations[0]));
        history->count -= 1U;
    }

    history->locations[history->count++] = *location;
    history->cursor = history->count - 1U;
    history->has_cursor = 1;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer navigation history back operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_navigation_history_back(
    UmiDeveloperNavigationHistory *history,
    UmiDeveloperProductivityLocation *out_location)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!history->has_cursor || history->cursor == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    history->cursor -= 1U;
    *out_location = history->locations[history->cursor];
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer navigation history forward operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_navigation_history_forward(
    UmiDeveloperNavigationHistory *history,
    UmiDeveloperProductivityLocation *out_location)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!history->has_cursor ||
        history->cursor + 1U >= history->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    history->cursor += 1U;
    *out_location = history->locations[history->cursor];
    history->revision += 1U;
    return UMI_STATUS_OK;
}
