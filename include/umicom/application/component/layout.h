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

typedef struct UmiApplicationComponentLayoutSlot {
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  UmiApplicationComponentRegion region;
  uint32_t order;
  uint32_t weight;
  int visible;
} UmiApplicationComponentLayoutSlot;

typedef struct UmiApplicationComponentLayout {
  char layout_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char title[UMI_APPLICATION_COMPONENT_TITLE_CAPACITY];
  UmiApplicationComponentLayoutSlot
      slots[UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY];
  size_t slot_count;
} UmiApplicationComponentLayout;

typedef struct UmiApplicationComponentWorkspace {
  UmiApplicationComponentFrontend frontend;
  UmiApplicationComponentInstance
      instances[UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY];
  UmiApplicationComponentRegion
      regions[UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY];
  size_t instance_count;
} UmiApplicationComponentWorkspace;

UmiStatus umi_application_component_layout_init(
    UmiApplicationComponentLayout *layout, const char *layout_id,
    const char *title);
UmiStatus umi_application_component_layout_add(
    UmiApplicationComponentLayout *layout, const char *component_id,
    const char *instance_id, UmiApplicationComponentRegion region,
    uint32_t weight);
UmiStatus umi_application_component_layout_validate(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentLayout *layout,
    UmiApplicationComponentFrontend frontend);
UmiStatus umi_application_component_layout_materialise(
    const UmiApplicationComponentRegistry *components,
    const UmiApplicationComponentFactoryRegistry *factories,
    const UmiApplicationComponentLayout *layout,
    UmiApplicationComponentFrontend frontend,
    UmiApplicationComponentWorkspace *out_workspace);
void umi_application_component_workspace_destroy(
    UmiApplicationComponentWorkspace *workspace);

#ifdef __cplusplus
}
#endif

#endif
