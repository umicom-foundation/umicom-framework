/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/selection.c
 *
 * PURPOSE:
 *   Resolve declarative component requests against version, maturity,
 *   multiplicity and frontend contracts before creating UI objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/selection.h"

#include <string.h>

#include "umicom/base/text.h"

/*
 * Initialise application component selection from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_component_selection_init(
    UmiApplicationComponentSelection *selection,
    UmiApplicationComponentFrontend frontend) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (selection != NULL) {
    (void)memset(selection, 0, sizeof(*selection));
    selection->frontend = frontend;
    selection->maximum_maturity = UMI_CAPABILITY_FOUNDATION;
  }
}

/*
 * Add application component selection only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_application_component_selection_add(
    UmiApplicationComponentSelection *selection, const char *component_id,
    const char *instance_id, int required) {
  UmiApplicationComponentSelectionItem *item;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (selection == NULL || component_id == NULL || component_id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (selection->count > UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (selection->count >= UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  item = &selection->items[selection->count];
  status = umi_text_copy(item->component_id, sizeof(item->component_id),
                         component_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(item->instance_id, sizeof(item->instance_id),
                           instance_id != NULL && instance_id[0] != '\0'
                               ? instance_id
                               : component_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK) {
    (void)memset(item, 0, sizeof(*item));
    return status;
  }
  item->required = required != 0;
  selection->count += 1U;
  return UMI_STATUS_OK;
}

/*
 * Provide the application component selection resolve operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_component_selection_resolve(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentSelection *selection,
    UmiApplicationComponentSelectionPlan *out_plan) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || selection == NULL || out_plan == NULL ||
      !umi_application_component_frontend_valid(selection->frontend))
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (selection->count > UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  (void)memset(out_plan, 0, sizeof(*out_plan));
  out_plan->frontend = selection->frontend;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < selection->count; ++index) {
    const UmiApplicationComponentSelectionItem *item = &selection->items[index];
    const UmiApplicationComponentContract *contract =
        umi_application_component_registry_find(registry, item->component_id);
    size_t previous;
    int available = contract != NULL &&
                    contract->definition->maturity <= selection->maximum_maturity &&
                    umi_application_component_contract_supports(
                        contract, selection->frontend);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!available) {
      /* Apply this branch only when its contract condition is satisfied. */
      if (item->required)
        return UMI_STATUS_UNAVAILABLE;
      out_plan->skipped_optional += 1U;
      continue;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < out_plan->count; ++previous) {
      /* Use the stable identifier comparison to choose the matching record or policy. */
      if (strcmp(out_plan->items[previous].instance_id, item->instance_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
      /* Apply this branch only when its contract condition is satisfied. */
      if (!contract->definition->multi_instance &&
          strcmp(out_plan->items[previous].contract->definition->component_id,
                 item->component_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
    out_plan->items[out_plan->count].contract = contract;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_text_copy(out_plan->items[out_plan->count].instance_id,
                      sizeof(out_plan->items[out_plan->count].instance_id),
                      item->instance_id) != UMI_STATUS_OK)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    out_plan->count += 1U;
  }
  return UMI_STATUS_OK;
}
