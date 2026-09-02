/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/instance.c
 *
 * PURPOSE:
 *   Coordinate component lifecycle transitions through registered frontend
 *   factories while retaining one portable instance identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/instance.h"

#include <string.h>

#include "umicom/base/text.h"

/*
 * Initialise application component instance from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_component_instance_create(
    const UmiApplicationComponentRegistry *components,
    const UmiApplicationComponentFactoryRegistry *factories,
    const UmiApplicationComponentInstanceRequest *request,
    UmiApplicationComponentInstance *out_instance) {
  const UmiApplicationComponentContract *contract;
  const UmiApplicationComponentFactoryDescriptor *factory;
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (components == NULL || factories == NULL || request == NULL ||
      out_instance == NULL || request->component_id == NULL ||
      request->instance_id == NULL || request->component_id[0] == '\0' ||
      request->instance_id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  contract = umi_application_component_registry_find(components,
                                                     request->component_id);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (contract == NULL)
    return UMI_STATUS_NOT_FOUND;
  /* Apply this branch only when its contract condition is satisfied. */
  if (!umi_application_component_contract_supports(contract, request->frontend))
    return UMI_STATUS_UNAVAILABLE;
  factory = umi_application_component_factory_registry_find(
      factories, contract->factory_id, request->frontend);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (factory == NULL)
    return UMI_STATUS_UNAVAILABLE;

  (void)memset(out_instance, 0, sizeof(*out_instance));
  status = umi_text_copy(out_instance->component_id,
                         sizeof(out_instance->component_id),
                         request->component_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(out_instance->instance_id,
                           sizeof(out_instance->instance_id),
                           request->instance_id);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  status = factory->create(contract, request->instance_id, factory->user_data,
                           &out_instance->handle);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK) {
    (void)memset(out_instance, 0, sizeof(*out_instance));
    return status;
  }
  out_instance->frontend = request->frontend;
  out_instance->lifecycle = UMI_APPLICATION_COMPONENT_CREATED;
  out_instance->contract = contract;
  out_instance->factory = factory;
  out_instance->revision = 1U;
  return UMI_STATUS_OK;
}

/*
 * Provide the application component instance activate operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_component_instance_activate(
    UmiApplicationComponentInstance *instance) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (instance == NULL || instance->factory == NULL || instance->handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (instance->lifecycle != UMI_APPLICATION_COMPONENT_CREATED &&
      instance->lifecycle != UMI_APPLICATION_COMPONENT_SUSPENDED)
    return UMI_STATUS_INVALID_STATE;
  status = instance->factory->activate != NULL
               ? instance->factory->activate(instance->handle,
                                             instance->factory->user_data)
               : UMI_STATUS_OK;
  instance->lifecycle = status == UMI_STATUS_OK
                            ? UMI_APPLICATION_COMPONENT_ACTIVE
                            : UMI_APPLICATION_COMPONENT_FAILED;
  instance->revision += 1U;
  return status;
}

/*
 * Provide the application component instance suspend operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_instance_suspend(
    UmiApplicationComponentInstance *instance) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (instance == NULL || instance->factory == NULL || instance->handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this operation only while the related capability or state is available. */
  if (instance->lifecycle != UMI_APPLICATION_COMPONENT_ACTIVE)
    return UMI_STATUS_INVALID_STATE;
  status = instance->factory->suspend != NULL
               ? instance->factory->suspend(instance->handle,
                                            instance->factory->user_data)
               : UMI_STATUS_OK;
  instance->lifecycle = status == UMI_STATUS_OK
                            ? UMI_APPLICATION_COMPONENT_SUSPENDED
                            : UMI_APPLICATION_COMPONENT_FAILED;
  instance->revision += 1U;
  return status;
}

/*
 * Provide the application component instance close operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_component_instance_close(
    UmiApplicationComponentInstance *instance) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (instance == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (instance->lifecycle == UMI_APPLICATION_COMPONENT_CLOSED)
    return UMI_STATUS_OK;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (instance->factory == NULL || instance->handle == NULL)
    return UMI_STATUS_INVALID_STATE;
  instance->factory->destroy(instance->handle, instance->factory->user_data);
  instance->handle = NULL;
  instance->lifecycle = UMI_APPLICATION_COMPONENT_CLOSED;
  instance->revision += 1U;
  return UMI_STATUS_OK;
}
