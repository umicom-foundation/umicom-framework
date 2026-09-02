/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/navigation_history.h
 *
 * PURPOSE:
 *   Retain cross-domain IDE navigation targets independently of any one editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_NAVIGATION_HISTORY_H
#define UMICOM_IDE_INTEGRATION_NAVIGATION_HISTORY_H
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ide navigation history data shared with callers of this public contract.
 */
typedef struct UmiIdeNavigationHistory UmiIdeNavigationHistory;

/**
 * Initialise ide navigation history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ide_navigation_history_create(
    UmiIdeNavigationHistory **out_history);
/**
 * Release or reset state held by ide navigation history so the same storage can be reused
 * safely.
 */
void umi_ide_navigation_history_destroy(UmiIdeNavigationHistory *history);

/**
 * Provide the ide navigation history visit operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_navigation_history_visit(
    UmiIdeNavigationHistory *history,
    const UmiIdeNavigationTarget *target);

/**
 * Provide the ide navigation history current operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_navigation_history_current(
    const UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target);

/**
 * Provide the ide navigation history back operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_navigation_history_back(
    UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target);

/**
 * Provide the ide navigation history forward operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_navigation_history_forward(
    UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target);

/**
 * Return the number of records represented by ide navigation history without changing
 * their state.
 */
size_t umi_ide_navigation_history_count(
    const UmiIdeNavigationHistory *history);

/**
 * Provide the ide navigation history revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ide_navigation_history_revision(
    const UmiIdeNavigationHistory *history);

#ifdef __cplusplus
}
#endif
#endif
