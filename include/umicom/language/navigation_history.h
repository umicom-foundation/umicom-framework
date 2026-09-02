/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language/navigation_history.h
 *
 * PURPOSE:
 *   Publish the public navigation history contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LANGUAGE_NAVIGATION_HISTORY_H
#define UMICOM_LANGUAGE_NAVIGATION_HISTORY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_LANGUAGE_NAVIGATION_CAPACITY 512U
/**
 * Represent the language navigation location data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageNavigationLocation{char uri[1024];uint32_t line;uint32_t column;char symbol[256];uint64_t revision;}UmiLanguageNavigationLocation;
/**
 * Represent the language navigation history data shared with callers of this public
 * contract.
 */
typedef struct UmiLanguageNavigationHistory UmiLanguageNavigationHistory;
/**
 * Initialise language navigation history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_navigation_history_create(UmiLanguageNavigationHistory **out_history);
/**
 * Release or reset state held by language navigation history so the same storage can be
 * reused safely.
 */
void umi_language_navigation_history_destroy(UmiLanguageNavigationHistory *history);
/**
 * Provide the language navigation history push operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_navigation_history_push(UmiLanguageNavigationHistory *history,const UmiLanguageNavigationLocation *location);
/**
 * Provide the language navigation history back operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_navigation_history_back(UmiLanguageNavigationHistory *history,UmiLanguageNavigationLocation *out_location);
/**
 * Provide the language navigation history forward operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_navigation_history_forward(UmiLanguageNavigationHistory *history,UmiLanguageNavigationLocation *out_location);
/**
 * Return the number of records represented by language navigation history without changing
 * their state.
 */
size_t umi_language_navigation_history_count(const UmiLanguageNavigationHistory *history);
#ifdef __cplusplus
}
#endif
#endif
