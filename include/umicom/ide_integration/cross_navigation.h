/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/cross_navigation.h
 *
 * PURPOSE:
 *   Provide one Framework-owned navigation service connecting Problems, Tests,
 *   Source Control, Debug, language intelligence, AI and DocumentCoordinator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_CROSS_NAVIGATION_H
#define UMICOM_IDE_INTEGRATION_CROSS_NAVIGATION_H

#include "umicom/ide_integration/bindings.h"
#include "umicom/ide_integration/debug_bridge.h"
#include "umicom/ide_integration/document_bridge.h"
#include "umicom/ide_integration/language_bridge.h"
#include "umicom/ide_integration/navigation_history.h"
#include "umicom/ide_integration/problem_bridge.h"
#include "umicom/ide_integration/productivity_bridge.h"
#include "umicom/ide_integration/source_control_bridge.h"
#include "umicom/ide_integration/test_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiIdeCrossNavigation {
    UmiIdeIntegrationBindings *bindings;
    UmiIdeNavigationHistory *history;
    UmiIdeDebugBridge debug_bridge;
    int debug_bridge_ready;
    uint64_t revision;
} UmiIdeCrossNavigation;

UmiStatus umi_ide_cross_navigation_init(
    UmiIdeCrossNavigation *navigation,
    UmiIdeIntegrationBindings *bindings);

void umi_ide_cross_navigation_deinit(
    UmiIdeCrossNavigation *navigation);

UmiStatus umi_ide_cross_navigation_set_debug_resolver(
    UmiIdeCrossNavigation *navigation,
    UmiIdeDebugFrameLocationResolver resolver,
    void *user_data);

UmiStatus umi_ide_cross_navigation_open(
    UmiIdeCrossNavigation *navigation,
    const UmiIdeNavigationTarget *target,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

UmiStatus umi_ide_cross_navigation_problem(
    UmiIdeCrossNavigation *navigation,
    size_t problem_index,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

UmiStatus umi_ide_cross_navigation_test(
    UmiIdeCrossNavigation *navigation,
    const char *test_item_id,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

UmiStatus umi_ide_cross_navigation_source_control(
    UmiIdeCrossNavigation *navigation,
    const char *path,
    uint32_t line,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

UmiStatus umi_ide_cross_navigation_symbol(
    UmiIdeCrossNavigation *navigation,
    const char *symbol_id,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

UmiStatus umi_ide_cross_navigation_diagnostic(
    UmiIdeCrossNavigation *navigation,
    size_t diagnostic_index,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

UmiStatus umi_ide_cross_navigation_debug_frame(
    UmiIdeCrossNavigation *navigation,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

UmiStatus umi_ide_cross_navigation_back(
    UmiIdeCrossNavigation *navigation,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

UmiStatus umi_ide_cross_navigation_forward(
    UmiIdeCrossNavigation *navigation,
    char *out_view_id,
    size_t view_id_capacity,
    size_t *out_offset);

#ifdef __cplusplus
}
#endif
#endif
