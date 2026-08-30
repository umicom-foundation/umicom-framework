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

void umi_developer_navigation_history_init(
    UmiDeveloperNavigationHistory *history)
{
    if (history == NULL) return;
    (void)memset(history, 0, sizeof(*history));
    history->revision = 1U;
}

UmiStatus umi_developer_navigation_history_push(
    UmiDeveloperNavigationHistory *history,
    const UmiDeveloperProductivityLocation *location)
{
    UmiStatus status;

    if (history == NULL || location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_productivity_location_validate(location);
    if (status != UMI_STATUS_OK) return status;

    if (history->has_cursor &&
        history->cursor + 1U < history->count) {
        history->count = history->cursor + 1U;
    }

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

UmiStatus umi_developer_navigation_history_back(
    UmiDeveloperNavigationHistory *history,
    UmiDeveloperProductivityLocation *out_location)
{
    if (history == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!history->has_cursor || history->cursor == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }

    history->cursor -= 1U;
    *out_location = history->locations[history->cursor];
    history->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_navigation_history_forward(
    UmiDeveloperNavigationHistory *history,
    UmiDeveloperProductivityLocation *out_location)
{
    if (history == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!history->has_cursor ||
        history->cursor + 1U >= history->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    history->cursor += 1U;
    *out_location = history->locations[history->cursor];
    history->revision += 1U;
    return UMI_STATUS_OK;
}
