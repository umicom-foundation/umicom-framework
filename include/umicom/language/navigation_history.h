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
typedef struct UmiLanguageNavigationLocation{char uri[1024];uint32_t line;uint32_t column;char symbol[256];uint64_t revision;}UmiLanguageNavigationLocation;
typedef struct UmiLanguageNavigationHistory UmiLanguageNavigationHistory;
UmiStatus umi_language_navigation_history_create(UmiLanguageNavigationHistory **out_history);
void umi_language_navigation_history_destroy(UmiLanguageNavigationHistory *history);
UmiStatus umi_language_navigation_history_push(UmiLanguageNavigationHistory *history,const UmiLanguageNavigationLocation *location);
UmiStatus umi_language_navigation_history_back(UmiLanguageNavigationHistory *history,UmiLanguageNavigationLocation *out_location);
UmiStatus umi_language_navigation_history_forward(UmiLanguageNavigationHistory *history,UmiLanguageNavigationLocation *out_location);
size_t umi_language_navigation_history_count(const UmiLanguageNavigationHistory *history);
#ifdef __cplusplus
}
#endif
#endif
