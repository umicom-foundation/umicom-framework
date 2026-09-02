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

/**
 * Represent the developer navigation history data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperNavigationHistory {
    UmiDeveloperProductivityLocation
        locations[UMI_DEVELOPER_NAVIGATION_HISTORY_CAPACITY];
    size_t count;
    size_t cursor;
    int has_cursor;
    uint64_t revision;
} UmiDeveloperNavigationHistory;

/**
 * Initialise developer navigation history from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_navigation_history_init(
    UmiDeveloperNavigationHistory *history);

/**
 * Provide the developer navigation history push operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_navigation_history_push(
    UmiDeveloperNavigationHistory *history,
    const UmiDeveloperProductivityLocation *location);

/**
 * Provide the developer navigation history back operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_navigation_history_back(
    UmiDeveloperNavigationHistory *history,
    UmiDeveloperProductivityLocation *out_location);

/**
 * Provide the developer navigation history forward operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_navigation_history_forward(
    UmiDeveloperNavigationHistory *history,
    UmiDeveloperProductivityLocation *out_location);

#ifdef __cplusplus
}
#endif

#endif
