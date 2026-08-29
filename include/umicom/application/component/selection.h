/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/selection.h
 *
 * PURPOSE:
 *   Resolve declarative component selections into a versioned, frontend-aware
 *   composition plan without application-specific factory code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_SELECTION_H
#define UMICOM_APPLICATION_COMPONENT_SELECTION_H

#include "umicom/application/component/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationComponentSelectionItem {
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  int required;
} UmiApplicationComponentSelectionItem;

typedef struct UmiApplicationComponentSelection {
  UmiApplicationComponentFrontend frontend;
  UmiCapabilityMaturity maximum_maturity;
  UmiApplicationComponentSelectionItem
      items[UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY];
  size_t count;
} UmiApplicationComponentSelection;

typedef struct UmiApplicationComponentSelectionPlanItem {
  const UmiApplicationComponentContract *contract;
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
} UmiApplicationComponentSelectionPlanItem;

typedef struct UmiApplicationComponentSelectionPlan {
  UmiApplicationComponentFrontend frontend;
  UmiApplicationComponentSelectionPlanItem
      items[UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY];
  size_t count;
  size_t skipped_optional;
} UmiApplicationComponentSelectionPlan;

void umi_application_component_selection_init(
    UmiApplicationComponentSelection *selection,
    UmiApplicationComponentFrontend frontend);
UmiStatus umi_application_component_selection_add(
    UmiApplicationComponentSelection *selection, const char *component_id,
    const char *instance_id, int required);
UmiStatus umi_application_component_selection_resolve(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentSelection *selection,
    UmiApplicationComponentSelectionPlan *out_plan);

#ifdef __cplusplus
}
#endif

#endif
