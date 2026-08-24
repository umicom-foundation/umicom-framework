/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/navigation_history.h
 *
 * PURPOSE:
 *   Retain cross-domain IDE navigation targets independently of any one editor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_NAVIGATION_HISTORY_H
#define UMICOM_IDE_INTEGRATION_NAVIGATION_HISTORY_H
#include "umicom/ide_integration/location.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeNavigationHistory UmiIdeNavigationHistory;

UmiStatus umi_ide_navigation_history_create(
    UmiIdeNavigationHistory **out_history);
void umi_ide_navigation_history_destroy(UmiIdeNavigationHistory *history);

UmiStatus umi_ide_navigation_history_visit(
    UmiIdeNavigationHistory *history,
    const UmiIdeNavigationTarget *target);

UmiStatus umi_ide_navigation_history_current(
    const UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target);

UmiStatus umi_ide_navigation_history_back(
    UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target);

UmiStatus umi_ide_navigation_history_forward(
    UmiIdeNavigationHistory *history,
    UmiIdeNavigationTarget *out_target);

size_t umi_ide_navigation_history_count(
    const UmiIdeNavigationHistory *history);

uint64_t umi_ide_navigation_history_revision(
    const UmiIdeNavigationHistory *history);

#ifdef __cplusplus
}
#endif
#endif
