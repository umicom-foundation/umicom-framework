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

/*
 * Initialise application component layout from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_component_layout_init(
    UmiApplicationComponentLayout *layout, const char *layout_id,
    const char *title) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (layout == NULL || layout_id == NULL || title == NULL ||
      layout_id[0] == '\0' || title[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(layout, 0, sizeof(*layout));
  status = umi_text_copy(layout->layout_id, sizeof(layout->layout_id), layout_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(layout->title, sizeof(layout->title), title);
  return status;
}

/*
 * Add application component layout only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_application_component_layout_add(
    UmiApplicationComponentLayout *layout, const char *component_id,
    const char *instance_id, UmiApplicationComponentRegion region,
    uint32_t weight) {
  UmiApplicationComponentLayoutSlot *slot;
  size_t index;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (layout == NULL || component_id == NULL || instance_id == NULL ||
      component_id[0] == '\0' || instance_id[0] == '\0' || weight == 0U ||
      region < UMI_APPLICATION_COMPONENT_REGION_PRIMARY ||
      region > UMI_APPLICATION_COMPONENT_REGION_FLOATING)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (layout->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (layout->slot_count >= UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < layout->slot_count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(layout->slots[index].instance_id, instance_id) == 0)
      return UMI_STATUS_ALREADY_EXISTS;
  }
  slot = &layout->slots[layout->slot_count];
  status = umi_text_copy(slot->component_id, sizeof(slot->component_id),
                         component_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(slot->instance_id, sizeof(slot->instance_id),
                           instance_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Check that application component layout satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_component_layout_validate(
    const UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentLayout *layout,
    UmiApplicationComponentFrontend frontend) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || layout == NULL || layout->layout_id[0] == '\0' ||
      layout->title[0] == '\0' || layout->slot_count == 0U ||
      !umi_application_component_frontend_valid(frontend))
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (layout->slot_count > UMI_APPLICATION_COMPONENT_LAYOUT_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < layout->slot_count; ++index) {
    const UmiApplicationComponentContract *contract =
        umi_application_component_registry_find(
            registry, layout->slots[index].component_id);
    size_t previous;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (contract == NULL)
      return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_application_component_contract_supports(contract, frontend))
      return UMI_STATUS_UNAVAILABLE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (previous = 0U; previous < index; ++previous) {
      /* Apply this branch only when its contract condition is satisfied. */
      if (!contract->definition->multi_instance &&
          strcmp(layout->slots[previous].component_id,
                 layout->slots[index].component_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}

/*
 * Release or reset state held by application component workspace so the same storage can
 * be reused safely.
 */
void umi_application_component_workspace_destroy(
    UmiApplicationComponentWorkspace *workspace) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (workspace != NULL) {
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (workspace->instance_count > 0U) {
      workspace->instance_count -= 1U;
      (void)umi_application_component_instance_close(
          &workspace->instances[workspace->instance_count]);
    }
    (void)memset(workspace, 0, sizeof(*workspace));
  }
}

/*
 * Provide the application component layout materialise operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_component_layout_materialise(
    const UmiApplicationComponentRegistry *components,
    const UmiApplicationComponentFactoryRegistry *factories,
    const UmiApplicationComponentLayout *layout,
    UmiApplicationComponentFrontend frontend,
    UmiApplicationComponentWorkspace *out_workspace) {
  size_t index;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (factories == NULL || out_workspace == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_component_layout_validate(components, layout, frontend);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  (void)memset(out_workspace, 0, sizeof(*out_workspace));
  out_workspace->frontend = frontend;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < layout->slot_count; ++index) {
    UmiApplicationComponentInstanceRequest request;
    request.component_id = layout->slots[index].component_id;
    request.instance_id = layout->slots[index].instance_id;
    request.frontend = frontend;
    status = umi_application_component_instance_create(
        components, factories, &request,
        &out_workspace->instances[out_workspace->instance_count]);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (status == UMI_STATUS_OK && layout->slots[index].visible)
      status = umi_application_component_instance_activate(
          &out_workspace->instances[out_workspace->instance_count]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
      /*
       * Protect caller-owned memory by checking that required state is available before it is
       * used.
       */
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
