/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/layout.h
 *
 * PURPOSE:
 *   Compose reusable component instances into portable workbench regions and
 *   materialise the same layout through any registered frontend adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_LAYOUT_H
#define UMICOM_APPLICATION_COMPONENT_LAYOUT_H

#include "umicom/application/component/selection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application component layout slot data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentLayoutSlot {
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  UmiApplicationComponentRegion region;
  uint32_t order;
  uint32_t weight;
  int visible;
} UmiApplicationComponentLayoutSlot;

/**
 * Represent the application component layout data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentLayout {
  char layout_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char title[UMI_APPLICATION_COMPONENT_TITLE_CAPACITY];
  UmiApplicationComponentLayoutSlot
      slots[UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY];
  size_t slot_count;
} UmiApplicationComponentLayout;

/**
 * Represent the application component workspace data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentWorkspace {
  UmiApplicationComponentFrontend frontend;
  UmiApplicationComponentInstance
      instances[UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY];
  UmiApplicationComponentRegion
      regions[UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY];
  size_t instance_count;
} UmiApplicationComponentWorkspace;

/**
 * Initialise application component layout from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_component_layout_init(
    UmiApplicationComponentLayout *layout, const char *layout_id,
    const char *title);
/**
 * Add application component layout only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_application_component_layout_add(
    UmiApplicationComponentLayout *layout, const char *component_id,
    const char *instance_id, UmiApplicationComponentRegion region,
    uint32_t weight);
/**
 * Check that application component layout satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_component_layout_validate(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentLayout *layout,
    UmiApplicationComponentFrontend frontend);
/**
 * Provide the application component layout materialise operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_component_layout_materialise(
    const UmiApplicationComponentRegistry *components,
    const UmiApplicationComponentFactoryRegistry *factories,
    const UmiApplicationComponentLayout *layout,
    UmiApplicationComponentFrontend frontend,
    UmiApplicationComponentWorkspace *out_workspace);
/**
 * Release or reset state held by application component workspace so the same storage can
 * be reused safely.
 */
void umi_application_component_workspace_destroy(
    UmiApplicationComponentWorkspace *workspace);

#ifdef __cplusplus
}
#endif

#endif
