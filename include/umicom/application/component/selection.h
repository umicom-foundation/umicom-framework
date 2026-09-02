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

/**
 * Represent the application component selection item data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentSelectionItem {
  char component_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
  int required;
} UmiApplicationComponentSelectionItem;

/**
 * Represent the application component selection data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentSelection {
  UmiApplicationComponentFrontend frontend;
  UmiCapabilityMaturity maximum_maturity;
  UmiApplicationComponentSelectionItem
      items[UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY];
  size_t count;
} UmiApplicationComponentSelection;

/**
 * Represent the application component selection plan item data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentSelectionPlanItem {
  const UmiApplicationComponentContract *contract;
  char instance_id[UMI_APPLICATION_COMPONENT_ID_CAPACITY];
} UmiApplicationComponentSelectionPlanItem;

/**
 * Represent the application component selection plan data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationComponentSelectionPlan {
  UmiApplicationComponentFrontend frontend;
  UmiApplicationComponentSelectionPlanItem
      items[UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY];
  size_t count;
  size_t skipped_optional;
} UmiApplicationComponentSelectionPlan;

/**
 * Initialise application component selection from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_component_selection_init(
    UmiApplicationComponentSelection *selection,
    UmiApplicationComponentFrontend frontend);
/**
 * Add application component selection only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_application_component_selection_add(
    UmiApplicationComponentSelection *selection, const char *component_id,
    const char *instance_id, int required);
/**
 * Provide the application component selection resolve operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_component_selection_resolve(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentSelection *selection,
    UmiApplicationComponentSelectionPlan *out_plan);

#ifdef __cplusplus
}
#endif

#endif
