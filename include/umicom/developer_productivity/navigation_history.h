/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/navigation_history.h
 *
 * PURPOSE:
 *   Retain bounded editor/navigation history for Back/Forward commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_NAVIGATION_HISTORY_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_NAVIGATION_HISTORY_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_NAVIGATION_HISTORY_CAPACITY 256U

typedef struct UmiDeveloperNavigationHistory {
    UmiDeveloperProductivityLocation
        locations[UMI_DEVELOPER_NAVIGATION_HISTORY_CAPACITY];
    size_t count;
    size_t cursor;
    int has_cursor;
    uint64_t revision;
} UmiDeveloperNavigationHistory;

void umi_developer_navigation_history_init(
    UmiDeveloperNavigationHistory *history);

UmiStatus umi_developer_navigation_history_push(
    UmiDeveloperNavigationHistory *history,
    const UmiDeveloperProductivityLocation *location);

UmiStatus umi_developer_navigation_history_back(
    UmiDeveloperNavigationHistory *history,
    UmiDeveloperProductivityLocation *out_location);

UmiStatus umi_developer_navigation_history_forward(
    UmiDeveloperNavigationHistory *history,
    UmiDeveloperProductivityLocation *out_location);

#ifdef __cplusplus
}
#endif

#endif
