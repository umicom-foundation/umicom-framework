/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/bundle.c
 *
 * PURPOSE:
 *   Define reusable common, Studio and Trader component recipes that prove
 *   applications can assemble Framework components without copying UI logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/bundle.h"

#include <string.h>

static const char *const COMMON_COMPONENTS[] = {
    "umicom.shell.activity-bar", "umicom.shell.command-palette",
    "umicom.shell.notifications", "umicom.shared.tasks",
    "umicom.shared.audit"};
static const UmiApplicationComponentRegion COMMON_REGIONS[] = {
    UMI_APPLICATION_COMPONENT_REGION_LEFT,
    UMI_APPLICATION_COMPONENT_REGION_FLOATING,
    UMI_APPLICATION_COMPONENT_REGION_RIGHT,
    UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
    UMI_APPLICATION_COMPONENT_REGION_RIGHT};

static const char *const STUDIO_COMPONENTS[] = {
    "umicom.development.explorer", "umicom.development.editor",
    "umicom.development.build", "umicom.development.debug",
    "umicom.development.testing", "umicom.development.source-control",
    "umicom.development.terminal", "umicom.development.designer"};
static const UmiApplicationComponentRegion STUDIO_REGIONS[] = {
    UMI_APPLICATION_COMPONENT_REGION_LEFT,
    UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
    UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
    UMI_APPLICATION_COMPONENT_REGION_RIGHT,
    UMI_APPLICATION_COMPONENT_REGION_LEFT,
    UMI_APPLICATION_COMPONENT_REGION_LEFT,
    UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
    UMI_APPLICATION_COMPONENT_REGION_PRIMARY};

static const char *const TRADER_COMPONENTS[] = {
    "umicom.trading.watchlist", "umicom.trading.chart",
    "umicom.trading.market-depth", "umicom.trading.order-ticket",
    "umicom.trading.orders", "umicom.trading.executions",
    "umicom.trading.portfolio", "umicom.trading.risk"};
static const UmiApplicationComponentRegion TRADER_REGIONS[] = {
    UMI_APPLICATION_COMPONENT_REGION_LEFT,
    UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
    UMI_APPLICATION_COMPONENT_REGION_RIGHT,
    UMI_APPLICATION_COMPONENT_REGION_RIGHT,
    UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
    UMI_APPLICATION_COMPONENT_REGION_BOTTOM,
    UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
    UMI_APPLICATION_COMPONENT_REGION_RIGHT};

static const UmiApplicationComponentBundle BUNDLES[] = {
    {"umicom.bundle.common", "Common Workbench",
     "Shared navigation, commands, notifications, tasks and audit surfaces.",
     COMMON_COMPONENTS, COMMON_REGIONS,
     sizeof(COMMON_COMPONENTS) / sizeof(COMMON_COMPONENTS[0])},
    {"umicom.bundle.studio", "Umicom Studio",
     "Framework-owned IDE exploration, editing, build, debug and tool surfaces.",
     STUDIO_COMPONENTS, STUDIO_REGIONS,
     sizeof(STUDIO_COMPONENTS) / sizeof(STUDIO_COMPONENTS[0])},
    {"umicom.bundle.trader", "Umicom Trader",
     "Framework-owned market, execution, portfolio and risk surfaces.",
     TRADER_COMPONENTS, TRADER_REGIONS,
     sizeof(TRADER_COMPONENTS) / sizeof(TRADER_COMPONENTS[0])}};

/*
 * Return the number of records represented by application component bundle without
 * changing their state.
 */
size_t umi_application_component_bundle_count(void) {
  return sizeof(BUNDLES) / sizeof(BUNDLES[0]);
}

/*
 * Find application component bundle while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationComponentBundle *umi_application_component_bundle_at(
    size_t index) {
  return index < umi_application_component_bundle_count() ? &BUNDLES[index]
                                                           : NULL;
}

/*
 * Find application component bundle while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationComponentBundle *umi_application_component_bundle_find(
    const char *bundle_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (bundle_id == NULL)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_bundle_count(); ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(BUNDLES[index].bundle_id, bundle_id) == 0)
      return &BUNDLES[index];
  }
  return NULL;
}

/*
 * Provide the application component bundle layout operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_bundle_layout(
    const UmiApplicationComponentBundle *bundle,
    UmiApplicationComponentLayout *out_layout) {
  size_t index;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (bundle == NULL || out_layout == NULL || bundle->bundle_id == NULL ||
      bundle->title == NULL || bundle->component_ids == NULL ||
      bundle->regions == NULL || bundle->component_count == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_component_layout_init(out_layout, bundle->bundle_id,
                                                 bundle->title);
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; status == UMI_STATUS_OK && index < bundle->component_count;
       ++index) {
    status = umi_application_component_layout_add(
        out_layout, bundle->component_ids[index], bundle->component_ids[index],
        bundle->regions[index],
        bundle->regions[index] == UMI_APPLICATION_COMPONENT_REGION_PRIMARY ? 3U
                                                                           : 1U);
  }
  return status;
}
