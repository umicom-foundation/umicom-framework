/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/layout.c
 *
 * PURPOSE:
 *   Validate portable layout recipes and materialise their component instances
 *   transactionally through a selected frontend factory set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/layout.h"

#include <string.h>

#include "umicom/base/text.h"

UmiStatus umi_application_component_layout_init(
    UmiApplicationComponentLayout *layout, const char *layout_id,
    const char *title) {
  UmiStatus status;
  if (layout == NULL || layout_id == NULL || title == NULL ||
      layout_id[0] == '\0' || title[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(layout, 0, sizeof(*layout));
  status = umi_text_copy(layout->layout_id, sizeof(layout->layout_id), layout_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(layout->title, sizeof(layout->title), title);
  return status;
}

UmiStatus umi_application_component_layout_add(
    UmiApplicationComponentLayout *layout, const char *component_id,
    const char *instance_id, UmiApplicationComponentRegion region,
    uint32_t weight) {
  UmiApplicationComponentLayoutSlot *slot;
  size_t index;
  UmiStatus status;
  if (layout == NULL || component_id == NULL || instance_id == NULL ||
      component_id[0] == '\0' || instance_id[0] == '\0' || weight == 0U ||
      region < UMI_APPLICATION_COMPONENT_REGION_PRIMARY ||
      region > UMI_APPLICATION_COMPONENT_REGION_FLOATING)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (layout->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  if (layout->slot_count >= UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  for (index = 0U; index < layout->slot_count; ++index) {
    if (strcmp(layout->slots[index].instance_id, instance_id) == 0)
      return UMI_STATUS_ALREADY_EXISTS;
  }
  slot = &layout->slots[layout->slot_count];
  status = umi_text_copy(slot->component_id, sizeof(slot->component_id),
                         component_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(slot->instance_id, sizeof(slot->instance_id),
                           instance_id);
  if (status != UMI_STATUS_OK) {
    (void)memset(slot, 0, sizeof(*slot));
    return status;
  }
  slot->region = region;
  slot->order = (uint32_t)layout->slot_count;
  slot->weight = weight;
  slot->visible = 1;
  layout->slot_count += 1U;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_component_layout_validate(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentLayout *layout,
    UmiApplicationComponentFrontend frontend) {
  size_t index;
  if (registry == NULL || layout == NULL || layout->layout_id[0] == '\0' ||
      layout->title[0] == '\0' || layout->slot_count == 0U ||
      !umi_application_component_frontend_valid(frontend))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (layout->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  for (index = 0U; index < layout->slot_count; ++index) {
    const UmiApplicationComponentContract *contract =
        umi_application_component_registry_find(
            registry, layout->slots[index].component_id);
    size_t previous;
    if (contract == NULL)
      return UMI_STATUS_NOT_FOUND;
    if (!umi_application_component_contract_supports(contract, frontend))
      return UMI_STATUS_UNAVAILABLE;
    for (previous = 0U; previous < index; ++previous) {
      if (!contract->definition->multi_instance &&
          strcmp(layout->slots[previous].component_id,
                 layout->slots[index].component_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}

void umi_application_component_workspace_destroy(
    UmiApplicationComponentWorkspace *workspace) {
  if (workspace != NULL) {
    while (workspace->instance_count > 0U) {
      workspace->instance_count -= 1U;
      (void)umi_application_component_instance_close(
          &workspace->instances[workspace->instance_count]);
    }
    (void)memset(workspace, 0, sizeof(*workspace));
  }
}

UmiStatus umi_application_component_layout_materialise(
    const UmiApplicationComponentRegistry *components,
    const UmiApplicationComponentFactoryRegistry *factories,
    const UmiApplicationComponentLayout *layout,
    UmiApplicationComponentFrontend frontend,
    UmiApplicationComponentWorkspace *out_workspace) {
  size_t index;
  UmiStatus status;
  if (factories == NULL || out_workspace == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_component_layout_validate(components, layout, frontend);
  if (status != UMI_STATUS_OK)
    return status;
  (void)memset(out_workspace, 0, sizeof(*out_workspace));
  out_workspace->frontend = frontend;
  for (index = 0U; index < layout->slot_count; ++index) {
    UmiApplicationComponentInstanceRequest request;
    request.component_id = layout->slots[index].component_id;
    request.instance_id = layout->slots[index].instance_id;
    request.frontend = frontend;
    status = umi_application_component_instance_create(
        components, factories, &request,
        &out_workspace->instances[out_workspace->instance_count]);
    if (status == UMI_STATUS_OK && layout->slots[index].visible)
      status = umi_application_component_instance_activate(
          &out_workspace->instances[out_workspace->instance_count]);
    if (status != UMI_STATUS_OK) {
      if (out_workspace->instances[out_workspace->instance_count].handle != NULL)
        (void)umi_application_component_instance_close(
            &out_workspace->instances[out_workspace->instance_count]);
      umi_application_component_workspace_destroy(out_workspace);
      return status;
    }
    out_workspace->regions[out_workspace->instance_count] =
        layout->slots[index].region;
    out_workspace->instance_count += 1U;
  }
  return UMI_STATUS_OK;
}
