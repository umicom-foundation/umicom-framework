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

void umi_application_component_selection_init(
    UmiApplicationComponentSelection *selection,
    UmiApplicationComponentFrontend frontend) {
  if (selection != NULL) {
    (void)memset(selection, 0, sizeof(*selection));
    selection->frontend = frontend;
    selection->maximum_maturity = UMI_CAPABILITY_FOUNDATION;
  }
}

UmiStatus umi_application_component_selection_add(
    UmiApplicationComponentSelection *selection, const char *component_id,
    const char *instance_id, int required) {
  UmiApplicationComponentSelectionItem *item;
  UmiStatus status;
  if (selection == NULL || component_id == NULL || component_id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  if (selection->count > UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  if (selection->count >= UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  item = &selection->items[selection->count];
  status = umi_text_copy(item->component_id, sizeof(item->component_id),
                         component_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(item->instance_id, sizeof(item->instance_id),
                           instance_id != NULL && instance_id[0] != '\0'
                               ? instance_id
                               : component_id);
  if (status != UMI_STATUS_OK) {
    (void)memset(item, 0, sizeof(*item));
    return status;
  }
  item->required = required != 0;
  selection->count += 1U;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_component_selection_resolve(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentSelection *selection,
    UmiApplicationComponentSelectionPlan *out_plan) {
  size_t index;
  if (registry == NULL || selection == NULL || out_plan == NULL ||
      !umi_application_component_frontend_valid(selection->frontend))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (selection->count > UMI_APPLICATION_COMPONENT_SELECTION_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  (void)memset(out_plan, 0, sizeof(*out_plan));
  out_plan->frontend = selection->frontend;
  for (index = 0U; index < selection->count; ++index) {
    const UmiApplicationComponentSelectionItem *item = &selection->items[index];
    const UmiApplicationComponentContract *contract =
        umi_application_component_registry_find(registry, item->component_id);
    size_t previous;
    int available = contract != NULL &&
                    contract->definition->maturity <= selection->maximum_maturity &&
                    umi_application_component_contract_supports(
                        contract, selection->frontend);
    if (!available) {
      if (item->required)
        return UMI_STATUS_UNAVAILABLE;
      out_plan->skipped_optional += 1U;
      continue;
    }
    for (previous = 0U; previous < out_plan->count; ++previous) {
      if (strcmp(out_plan->items[previous].instance_id, item->instance_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
      if (!contract->definition->multi_instance &&
          strcmp(out_plan->items[previous].contract->definition->component_id,
                 item->component_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
    out_plan->items[out_plan->count].contract = contract;
    if (umi_text_copy(out_plan->items[out_plan->count].instance_id,
                      sizeof(out_plan->items[out_plan->count].instance_id),
                      item->instance_id) != UMI_STATUS_OK)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    out_plan->count += 1U;
  }
  return UMI_STATUS_OK;
}
