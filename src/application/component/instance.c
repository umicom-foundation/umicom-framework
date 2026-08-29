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

UmiStatus umi_application_component_instance_create(
    const UmiApplicationComponentRegistry *components,
    const UmiApplicationComponentFactoryRegistry *factories,
    const UmiApplicationComponentInstanceRequest *request,
    UmiApplicationComponentInstance *out_instance) {
  const UmiApplicationComponentContract *contract;
  const UmiApplicationComponentFactoryDescriptor *factory;
  UmiStatus status;
  if (components == NULL || factories == NULL || request == NULL ||
      out_instance == NULL || request->component_id == NULL ||
      request->instance_id == NULL || request->component_id[0] == '\0' ||
      request->instance_id[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  contract = umi_application_component_registry_find(components,
                                                     request->component_id);
  if (contract == NULL)
    return UMI_STATUS_NOT_FOUND;
  if (!umi_application_component_contract_supports(contract, request->frontend))
    return UMI_STATUS_UNAVAILABLE;
  factory = umi_application_component_factory_registry_find(
      factories, contract->factory_id, request->frontend);
  if (factory == NULL)
    return UMI_STATUS_UNAVAILABLE;

  (void)memset(out_instance, 0, sizeof(*out_instance));
  status = umi_text_copy(out_instance->component_id,
                         sizeof(out_instance->component_id),
                         request->component_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(out_instance->instance_id,
                           sizeof(out_instance->instance_id),
                           request->instance_id);
  if (status != UMI_STATUS_OK)
    return status;
  status = factory->create(contract, request->instance_id, factory->user_data,
                           &out_instance->handle);
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

UmiStatus umi_application_component_instance_activate(
    UmiApplicationComponentInstance *instance) {
  UmiStatus status;
  if (instance == NULL || instance->factory == NULL || instance->handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
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

UmiStatus umi_application_component_instance_suspend(
    UmiApplicationComponentInstance *instance) {
  UmiStatus status;
  if (instance == NULL || instance->factory == NULL || instance->handle == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
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

UmiStatus umi_application_component_instance_close(
    UmiApplicationComponentInstance *instance) {
  if (instance == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (instance->lifecycle == UMI_APPLICATION_COMPONENT_CLOSED)
    return UMI_STATUS_OK;
  if (instance->factory == NULL || instance->handle == NULL)
    return UMI_STATUS_INVALID_STATE;
  instance->factory->destroy(instance->handle, instance->factory->user_data);
  instance->handle = NULL;
  instance->lifecycle = UMI_APPLICATION_COMPONENT_CLOSED;
  instance->revision += 1U;
  return UMI_STATUS_OK;
}
